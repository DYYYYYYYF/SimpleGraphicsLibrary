#include "OpenGLShader.h"
#include "Platform/File/File.h"
#include <Logger.hpp>

OpenGLShader::OpenGLShader() {
	ProgramID_ = NULL;

}

OpenGLShader::~OpenGLShader() {
	Unload();
}

bool OpenGLShader::Load(const std::string& path) {
	// TODO:从配置文件读取具体配置
	std::string vertFile = "../Assets/Shaders/Sources/Builtin/Shader.Builtin.vert";
	if (!AddStage(vertFile, ShaderStage::eVertex)) {
		return false;
	}

	std::string fragFile = "../Assets/Shaders/Sources/Builtin/Shader.Builtin.frag";
	if (!AddStage(fragFile, ShaderStage::eFragment)) {
		return false;
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
	return true;
}

void OpenGLShader::Unload() {
	if (ProgramID_ != NULL) {
		for (auto& Stage : ShaderStages_) {
			glDetachShader(ProgramID_, Stage.second);
			glDeleteShader(Stage.second);
		}

		glDeleteProgram(ProgramID_);
	}
}

void OpenGLShader::Bind() {
	if (ProgramID_ != 0){
		glUseProgram(ProgramID_);
	}
}

void OpenGLShader::Unbind() {
	glUseProgram(0);
}

GLint OpenGLShader::GetUniformLocation(const std::string& name) const {
	return glGetUniformLocation(ProgramID_, name.c_str());
}

bool OpenGLShader::AddStage(const std::string& source, ShaderStage stage) {
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

bool OpenGLShader::CompileShader(const std::string& source, GLuint& Obj)
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
void OpenGLShader::SetInt(const std::string& name, int value){ glUniform1i(GetUniformLocation(name), value); }
void OpenGLShader::SetFloat(const std::string& name, float value){ glUniform1f(GetUniformLocation(name), value); }
void OpenGLShader::SetVec2(const std::string& name, const FVector2& value){ glUniform2fv(GetUniformLocation(name), 1, value.data()); }
void OpenGLShader::SetVec3(const std::string& name, const FVector3& value){ glUniform3fv(GetUniformLocation(name), 1, value.data()); }
void OpenGLShader::SetVec4(const std::string& name, const FVector4& value){ glUniform4fv(GetUniformLocation(name), 1, value.data()); }
// GL_FALSE这里意味着cloumn-major 方式读取
void OpenGLShader::SetMat3(const std::string& name, const FMatrix3& value){ glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, value.data()); }
void OpenGLShader::SetMat4(const std::string& name, const FMatrix4& value) { glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value.data()); }