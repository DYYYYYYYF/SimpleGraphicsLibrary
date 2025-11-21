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

GLShader::GLShader(const std::string& AssetPath) {
	ProgramID_ = NULL;

	// 加载Shader
	if (!Load(AssetPath)) {
		return;
	}

	IsValid_ = true;
}

GLShader::~GLShader() {
	Unload();
}

bool GLShader::Load(const std::string& AssetPath) {
	// 读取配置
	File ShaderAsset(SHADER_CONFIG_PATH + AssetPath);
	if (!ShaderAsset.IsExist()) {
		return false;
	}

	JsonObject Content = JsonObject(ShaderAsset.ReadBytes());
	Name_ = Content.Get("Name").GetString();

	// Stages
	JsonObject StageObj = Content.Get("Stages");
	if (StageObj.IsArray() && StageObj.Size() > 0) {
		for (int i = 0; i < StageObj.Size(); ++i) {
			// 具体的阶段 vert geom frag comp
			JsonObject Stage = StageObj.ArrayItemAt(i);
			if (!Stage.IsObject()) {
				continue;
			}

			// 添加阶段
			std::string StageName = Stage.Get("Name").GetString();
			std::string StageSource = "../Assets/Shaders/Sources" + Stage.Get("Source").GetString();

			if (StageName.compare("vert") == 0) {
				if (!AddStage(StageSource, ShaderStage::eVertex)) {
					return false;
				}
			}
			else if (StageName.compare("geom") == 0) {
				LOG_WARN << "Engine not support geom shader yet!";
				continue;
			}
			else if (StageName.compare("frag") == 0) {
				if (!AddStage(StageSource, ShaderStage::eFragment)) {
					continue;
				}
			}
			else if (StageName.compare("comp") == 0) {
				LOG_WARN << "Engine not support comp shader yet!";
				continue;
			}
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

	Bind();

	LOG_DEBUG << "Shader '" << Name_ << "' loaded.";
	IsValid_ = true;
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
	const char* blockName = "MaterialBlock";
	GLuint blockIndex = glGetUniformBlockIndex(program, blockName);
	if (blockIndex == GL_INVALID_INDEX) {
		LOG_ERROR << "Shader missing MaterialBlock!";
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