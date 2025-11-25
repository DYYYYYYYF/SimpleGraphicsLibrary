#include "GLShader.h"
#include "GLMaterial.h"
#include "Platform/File/JsonObject.h"
#include "Resource/Manager/ResourceManager.h"
#include <Logger.hpp>

GLShader::GLShader() {
	ProgramID_ = NULL;
	Name_ = "";
	IsValid_ = false;
}

GLShader::GLShader(const ShaderDesc& Desc) {
	if (!Load(Desc)) {
		return;
	}

	IsValid_ = true;
}

GLShader::~GLShader() {
	Unload();
}

bool GLShader::Load(const ShaderDesc& Desc) {
	Name_ = Desc.Name;
	for (const auto& StageDesc : Desc.Stages) {
		// 添加阶段
		const ShaderStage Type = StageDesc.first;
		const std::string& Source = StageDesc.second;
		std::string StageSource = SHADER_ASSET_PATH + Source;

		if (!AddStage(StageSource, Type)) {
			continue;
		}
	}

	// 初始化Buffer
	glGenBuffers(1, &MaterialUBO_);
	glBindBuffer(GL_UNIFORM_BUFFER, MaterialUBO_);

	// 初始化ShaderProgram
	ProgramID_ = glCreateProgram();
	for (auto Stage : ShaderStages_) {
		glAttachShader(ProgramID_, Stage.second);
	}
	glLinkProgram(ProgramID_);

	// 检测连接结果
	GLint Success;
	glGetProgramiv(ProgramID_, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(ProgramID_, sizeof(ErrorLog), NULL, ErrorLog);
		LOG_ERROR << "Invalid shader program: '" << ErrorLog << "'.";
		return false;
	}

	glValidateProgram(ProgramID_);
	glGetProgramiv(ProgramID_, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(ProgramID_, sizeof(ErrorLog), NULL, ErrorLog);
		LOG_ERROR << "Invalid shader program: '" << ErrorLog << "'.";
		return false;
	}

	// 反射Uniform
	ReflectUnifromBlock();
	// 反射后可以知道BlockSize，初始化Buffer大小
	glBufferData(GL_UNIFORM_BUFFER, MaterialLayout_.blockSize, nullptr, GL_DYNAMIC_DRAW);

	// 绑定Shader
	Bind();

	LOG_DEBUG << "Shader '" << Name_ << "' loaded.";
	return true;
}

void GLShader::Unload() {
	if (ProgramID_ != NULL) {
		for (auto& Stage : ShaderStages_) {
			glDetachShader(ProgramID_, Stage.second);
			glDeleteShader(Stage.second);
		}

		glDeleteProgram(ProgramID_);
	}


	if (MaterialUBO_ != NULL) {
		glDeleteBuffers(1, &MaterialUBO_);
	}

	LOG_DEBUG << "Shader '" << Name_ << "' unloaded.";
}

void GLShader::Bind() {
	if (ProgramID_ != 0){
		glUseProgram(ProgramID_);
	}
}

void GLShader::Unbind() {
	glUseProgram(0);
}

GLint GLShader::GetUniformLocation(const std::string& name) const {
	return glGetUniformLocation(ProgramID_, name.c_str());
}

bool GLShader::AddStage(const std::string& source, ShaderStage stage) {
	GLuint shaderObj = 0;
	switch (stage)
	{
	case ShaderStage::eVertex: {
		shaderObj = glCreateShader(GL_VERTEX_SHADER);
	}
		break;
	case ShaderStage::eFragment: {
		shaderObj = glCreateShader(GL_FRAGMENT_SHADER);
	}
		break;
	default:
		LOG_ERROR << "Unknow shader stage: " << (int)stage;
		return false;
	}

	if (!CompileShader(source, shaderObj)) {
		glDeleteShader(shaderObj);
		LOG_ERROR << "Error compiling shader '" << source << "' Type: " << (int)stage;
		return false;
	}

	ShaderStages_[stage] = shaderObj;
	return true;
}

bool GLShader::CompileShader(const std::string& source, GLuint& Obj)
{
	File shaderSrc(source);
	if (!shaderSrc.IsExist()) {
		LOG_WARN << "Shader file not exist!";
		return false;
	}

	const std::string& context = shaderSrc.ReadBytes();
	const char* data = context.c_str();
	if (!data) {
		return false;
	}

	glShaderSource(Obj, 1, &data, NULL);
	glCompileShader(Obj);

	// 检查编译结果
	GLint success;
	glGetShaderiv(Obj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(Obj, 1024, NULL, InfoLog);
		// 删除结尾换行
		size_t len = strlen(InfoLog);
		if (len > 0) {
			InfoLog[len - 1] = ' ';
		}
		
		LOG_ERROR << "Compile info: " << InfoLog;
		return false;
	}

	return true;
}

// -------------------------------- 设置Uniform ----------------------------------
void GLShader::SetInt(const std::string& name, int value){ glUniform1i(GetUniformLocation(name), value); }
void GLShader::SetFloat(const std::string& name, float value){ glUniform1f(GetUniformLocation(name), value); }
void GLShader::SetVec2(const std::string& name, const FVector2& value){ glUniform2fv(GetUniformLocation(name), 1, value.data()); }
void GLShader::SetVec3(const std::string& name, const FVector3& value){ glUniform3fv(GetUniformLocation(name), 1, value.data()); }
void GLShader::SetVec4(const std::string& name, const FVector4& value){ glUniform4fv(GetUniformLocation(name), 1, value.data()); }
// GL_FALSE这里意味着cloumn-major 方式读取
void GLShader::SetMat3(const std::string& name, const FMatrix3& value){ glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, value.data()); }
void GLShader::SetMat4(const std::string& name, const FMatrix4& value) { glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value.data()); }

void GLShader::UploadMaterial(const IMaterial& Mat) {
	const ShaderUniformLayout& layout = MaterialLayout_;

	// 构建 CPU 端 buffer
	std::vector<uint8_t> buffer(layout.blockSize);

	// 遍历 material 的参数
	for (auto& kv : Mat.GetUniforms())
	{
		const std::string& name = kv.first;
		const MaterialValue& value = kv.second;

		if (!layout.uniforms.count(name))
			continue;

		const UniformInfo& info = layout.uniforms.at(name);

		// 写入对应的 offset
		memcpy(buffer.data() + info.offset, value.data.data(), info.size);
	}

	// 上传 GPU
	glBindBuffer(GL_UNIFORM_BUFFER, MaterialUBO_);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, layout.blockSize, buffer.data());
	glBindBufferBase(GL_UNIFORM_BUFFER, layout.binding, MaterialUBO_);
}

void GLShader::ReflectUnifromBlock() {
	GLuint program = ProgramID_;

	// 1. 查询 uniform block blockIndex
	const char* blockName = "MaterialUBO";
	GLuint blockIndex = glGetUniformBlockIndex(program, blockName);
	if (blockIndex == GL_INVALID_INDEX) {
		LOG_ERROR << "Shader missing MaterialUBO!";
		return;
	}

	MaterialLayout_.blockIndex = blockIndex;

	// 2. 获取 block 的大小
	GLint blockSize = 0;
	glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	MaterialLayout_.blockSize = blockSize;

	// 3. 获取 block 内包含的 uniform 数量
	GLint numUniforms = 0;
	glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniforms);

	// 4. 获取 block 中 uniform 的索引
	std::vector<GLint> uniformIndices(numUniforms);
	glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices.data());

	// 5. 查询每个 uniform 的名称、类型、offset 等
	for (int i = 0; i < numUniforms; i++)
	{
		char name[256];
		GLsizei length = 0;
		GLenum type = 0;
		GLint size = 0;

		GLuint index = uniformIndices[i];

		// uniform 名字
		glGetActiveUniform(program, index, sizeof(name), &length, &size, &type, name);

		// 查询 offset
		GLint offset = 0;
		glGetActiveUniformsiv(program, 1, &index, GL_UNIFORM_OFFSET, &offset);

		UniformInfo info;
		info.offset = offset;
		info.type = MapStd140Type(type);
		info.size = ComputeTypeSize(info.type);

		MaterialLayout_.uniforms[name] = info;
	}

	// 6. 查询绑定点(binding = X)
	GLint binding = 0;
	glGetActiveUniformBlockiv(program, blockIndex, GL_UNIFORM_BLOCK_BINDING, &binding);
	MaterialLayout_.binding = binding;
}

MaterialValue::Type GLShader::MapStd140Type(GLenum Type) {
	switch (Type)
	{
	case GL_FLOAT:          return MaterialValue::Type::Float;
	case GL_FLOAT_VEC2:     return MaterialValue::Type::Vector2;
	case GL_FLOAT_VEC3:     return MaterialValue::Type::Vector3; // vec3 在 std140 中按 vec4 对齐
	case GL_FLOAT_VEC4:     return MaterialValue::Type::Vector4;
	case GL_FLOAT_MAT4:     return MaterialValue::Type::Matrix4;
	default:
		LOG_ERROR << "Unsupported UBO type: " << (int)Type;
		return MaterialValue::Type::Vector4;
	}
}

int GLShader::ComputeTypeSize(MaterialValue::Type Type)
{
	switch (Type)
	{
	case MaterialValue::Type::Float:          return 4;
	case MaterialValue::Type::Vector2:     return 8;
	case MaterialValue::Type::Vector3:     return 16; // vec3 在 std140 中按 vec4 对齐
	case MaterialValue::Type::Vector4:     return 16;
	case MaterialValue::Type::Matrix4:     return 64;
	default:
		LOG_ERROR << "Unsupported UBO type: " << (int)Type;
		return 16;
	}
}