#include "photon.hpp"

void Photon::Camera::init(asIScriptModule *module) {
	type = module->GetTypeInfoByDecl("Camera");
	assert(type);
	ctx = module->GetEngine()->CreateContext();
	assert(ctx);
	asIScriptFunction *factory = type->GetFactoryByDecl("Camera @Camera()");
	assert(factory);
	ctx->Prepare(factory);
	if(ctx->Execute() == asEXECUTION_FINISHED) {
		obj = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
		obj->AddRef();
	}
	ctx->Unprepare();
}

void Photon::Camera::deinit() {
	if(obj) {
		obj->Release();
	}
	ctx->Release();
}

void Photon::Camera::update(float dt, math::vec2 dmouse) {
	static asIScriptFunction *func = type->GetMethodByDecl("void update(float, vec2)");
	assert(func);
	ctx->Prepare(func);
	ctx->SetObject(obj);
	ctx->SetArgFloat(0, dt);
	ctx->SetArgObject(1, &dmouse);
	ctx->Execute();
	ctx->Unprepare();
}

math::mat4 Photon::Camera::getViewTransform() {
	static asIScriptFunction *func = type->GetMethodByDecl("mat4 getViewTransform()");
	assert(func);
	ctx->Prepare(func);
	ctx->SetObject(obj);
	math::mat4 viewtransform;
	if(ctx->Execute() == asEXECUTION_FINISHED) {
		viewtransform = *reinterpret_cast<math::mat4*>(ctx->GetReturnObject());
	}
	ctx->Unprepare();
	return viewtransform;
}

math::mat4 Photon::Camera::getProjectionMatrix() {
	static asIScriptFunction *func = type->GetMethodByDecl("mat4 getProjectionMatrix()");
	assert(func);
	ctx->Prepare(func);
	ctx->SetObject(obj);
	math::mat4 projection;
	if(ctx->Execute() == asEXECUTION_FINISHED) {
		projection = *reinterpret_cast<math::mat4*>(ctx->GetReturnObject());
	}
	ctx->Unprepare();
	return projection;
}