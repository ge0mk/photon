#include "photon.hpp"

void Photon::ScriptGUI::init(asIScriptModule *module) {
	type = module->GetTypeInfoByDecl("GUI");
	assert(type);
	ctx = module->GetEngine()->CreateContext();
	assert(ctx);
	asIScriptFunction *factory = type->GetFactoryByDecl("GUI @GUI()");
	assert(factory);
	ctx->Prepare(factory);
	if(ctx->Execute() == asEXECUTION_FINISHED) {
		obj = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
		obj->AddRef();
	}
	ctx->Unprepare();
}

void Photon::ScriptGUI::deinit() {
	if(obj) {
		obj->Release();
	}
	ctx->Release();
}

void Photon::ScriptGUI::render() {
	static asIScriptFunction *func = type->GetMethodByDecl("void render()");
	assert(func);
	ctx->Prepare(func);
	ctx->SetObject(obj);
	ctx->Execute();
	ctx->Unprepare();
}