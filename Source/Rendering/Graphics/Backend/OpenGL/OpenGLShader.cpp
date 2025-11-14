#include "OpenGLShader.h"
#include "Platform/File/File.h"
#include <Logger.hpp>

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
	ShaderProgram_ = glCreateProgram();
	for (auto Stage : ShaderStages_) {
		glAttachShader(ShaderProgram_, Stage.second);
	}
	glLinkProgram(ShaderProgram_);

	// 检测连接结果
	GLint Success;
	glGetProgramiv(ShaderProgram_, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(ShaderProgram_, sizeof(ErrorLog), NULL, ErrorLog);
		LOG_ERROR << "Invalid shader program: '" << ErrorLog << "'.";
		return false;
	}

	glValidateProgram(ShaderProgram_);
	glGetProgramiv(ShaderProgram_, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(ShaderProgram_, sizeof(ErrorLog), NULL, ErrorLog);
		LOG_ERROR << "Invalid shader program: '" << ErrorLog << "'.";
		return false;
	}

	Use();
	return true;
}

void OpenGLShader::Unload() {
	if (ShaderProgram_) {
		for (auto& Stage : ShaderStages_) {
			glDetachShader(ShaderProgram_, Stage.second);
			glDeleteShader(Stage.second);
		}
	}
}

void OpenGLShader::Use() {
	glUseProgram(ShaderProgram_);
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