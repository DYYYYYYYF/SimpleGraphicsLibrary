#include "GLShader.h"
#include "Platform/File/JsonObject.h"
#include <Logger.hpp>

GLShader::GLShader() {
	ProgramID_ = NULL;
	Name_ = "";
	IsValid_ = false;
}

GLShader::GLShader(const std::string& AssetPath) {
	ProgramID_ = NULL;
	IsValid_ = false;

	// 加载Shader
	Load(AssetPath);
}

GLShader::~GLShader() {
	Unload();
}

bool GLShader::Load(const std::string& AssetPath) {
	File ShaderFile("../Assets/Shaders/Configs" + AssetPath);
	if (!ShaderFile.IsExist()) {
		return false;
	}

	JsonObject ShaderObj(ShaderFile);
	if (!ShaderObj.IsObject()) {
		return false;
	}

	Name_ = ShaderObj.Get("Name").GetString();

	// Stages
	JsonObject StageObj = ShaderObj.Get("Stages");
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