#include <iostream>
#include <File/JsonObject.h>

void Warning();

int main() {
	Warning();
	std::cout << "\n=== 测试扩展方法 ===" << std::endl;

	// 测试类型检查
	JsonObject nullObj("null");
	std::cout << "IsNull: " << nullObj.IsNull() << std::endl;

	// 测试便捷设置方法
	JsonObject obj(JsonObject::Type::eObject);
	obj.SetString("name", "测试");
	obj.SetInt("age", 25);
	obj.SetFloat("score", 95.5f);
	obj.SetBool("active", true);

	std::cout << "对象: " << obj.Dump(2) << std::endl;

	// 测试HasKey
	std::cout << "HasKey('name'): " << obj.HasKey("name") << std::endl;
	std::cout << "HasKey('missing'): " << obj.HasKey("missing") << std::endl;

	// 测试GetKeys
	auto keys = obj.GetKeys();
	std::cout << "所有键: ";
	for (const auto& key : keys) {
		std::cout << key << " ";
	}
	std::cout << std::endl;

	// 测试数组推入
	JsonObject arr(JsonObject::Type::eArray);
	arr.ArrayPush(JsonObject("\"item1\""));
	arr.ArrayPush(JsonObject("\"item2\""));
	arr.ArrayPush(JsonObject("\"item3\""));
	std::cout << "数组: " << arr.Dump() << std::endl;

	// 测试数组删除
	arr.ArrayRemoveAt(1);
	std::cout << "删除后: " << arr.Dump() << std::endl;

	// 测试克隆
	JsonObject cloned = obj.Clone();
	std::cout << "克隆对象: " << cloned.Dump(2) << std::endl;

	// 测试合并
	JsonObject obj2(JsonObject::Type::eObject);
	obj2.SetString("email", "test@example.com");
	obj2.SetString("name", "新名字");  // 会覆盖原有的
	std::cout << "合并后: " << obj.Dump(2) << std::endl;

	obj2.SetArray("scores", std::vector<int>{90, 85, 88});
	obj2.WriteInt("Window.Height", 100);
	obj2.WriteString("Window.Title", "测试窗口");

	obj2.SetVector3("Position", FVector3(1.0f, 2.0f, 3.0f));
	obj2.SetMatrix4("Transform", FMatrix4::Identity());

	int Hei = obj2.ReadInt("Window.Height", 0);
	std::cout << "Window.Height: " << Hei << std::endl;

	obj.Merge(obj2);
	File WrittedFile("./JsonFile.json");
	obj.SaveToFile(WrittedFile);

	return 0;
}

void Warning() {
	std::cout << "If the terminal console error code, please set system encode to utf-8. Thanks!\n";
}