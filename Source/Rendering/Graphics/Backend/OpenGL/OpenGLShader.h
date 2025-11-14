#include "Graphics/RHI/IShader.h"
#include "glad/glad.h"

#include <unordered_map>

class OpenGLShader : public IShader {
public:
	virtual bool Load(const std::string& path) override;
	virtual void Use() override;

private:
	bool AddStage(const std::string& source, ShaderStage stage);
	bool CompileShader(const std::string& source, GLuint& Obj);

private:
	GLuint ShaderProgram_;
	std::unordered_map<ShaderStage, GLuint> ShaderStages_;

};