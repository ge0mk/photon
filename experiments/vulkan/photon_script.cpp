#include "photon.hpp"

static void vec2FillConstructor(float v, math::vec2 *self) {new(self) math::vec2(v);}
static void vec2Copyvec2Constructor(const math::vec2 &xy, math::vec4 *self) {new(self) math::vec2(xy);}
static void vec2Copyvec3Constructor(const math::vec3 &other, math::vec4 *self) {new(self) math::vec2(other.xy);}
static void vec2Copyvec4Constructor(const math::vec4 &other, math::vec4 *self) {new(self) math::vec2(other.xy);}
static void vec2InitConstructor(float x, float y, math::vec2 *self) {new(self) math::vec2(x, y);}
static void vec2ListConstructor(float *list, math::vec2 *self) {new(self) math::vec2(list[0], list[1]);}

static void vec3FillConstructor(float v, math::vec3 *self) {new(self) math::vec3(v);}
static void vec3Copyvec2floatConstructor(const math::vec2 &xy, float z, math::vec4 *self) {new(self) math::vec3(xy, z);}
static void vec3Copyvec3Constructor(const math::vec3 &other, math::vec4 *self) {new(self) math::vec3(other);}
static void vec3Copyvec4Constructor(const math::vec4 &other, math::vec4 *self) {new(self) math::vec3(other.xyz);}
static void vec3InitConstructor(float x, float y, float z, math::vec3 *self) {new(self) math::vec3(x, y, z);}
static void vec3ListConstructor(float *list, math::vec3 *self) {new(self) math::vec3(list[0], list[1], list[2]);}

static void vec4FillConstructor(float v, math::vec4 *self) {new(self) math::vec4(v);}
static void vec4Copy2vec2Constructor(const math::vec2 &xy, const math::vec2 &zw, math::vec4 *self) {new(self) math::vec4(xy, zw);}
static void vec4Copyvec22floatConstructor(const math::vec2 &xy, float z, float w, math::vec4 *self) {new(self) math::vec4(xy, z, w);}
static void vec4Copyvec3floatConstructor(const math::vec3 &xyz, float w, math::vec4 *self) {new(self) math::vec4(xyz, w);}
static void vec4Copyvec4Constructor(const math::vec4 &other, math::vec4 *self) {new(self) math::vec4(other);}
static void vec4InitConstructor(float x, float y, float z, float w, math::vec4 *self) {new(self) math::vec4(x, y, z, w);}
static void vec4ListConstructor(float *list, math::vec4 *self) {new(self) math::vec4(list[0], list[1], list[2], list[3]);}

static void mat4CopyConstructor(const math::mat4 &other, math::mat4 *self) {new(self) math::mat4(other);}
static void mat4DefaultConstructor(math::mat4 *self) {new(self) math::mat4();}
static void mat4FillConstructor(float v, math::mat4 *self) {new(self) math::mat4(v);}
static void mat4ListConstructor(float *list, math::mat4 *self) {new(self) math::mat4(list[0], list[1],list[2], list[3], list[4], list[5],list[6], list[7], list[8], list[9],list[10], list[11], list[12], list[13],list[14], list[15]);}

void registerVec2(asIScriptEngine *engine) {
	engine->RegisterObjectType("vec2", sizeof(math::vec2), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<math::vec2>() | asOBJ_APP_CLASS_ALLFLOATS);
	engine->RegisterObjectProperty("vec2", "float x", asOFFSET(math::vec2, x));
	engine->RegisterObjectProperty("vec2", "float y", asOFFSET(math::vec2, y));
	engine->RegisterObjectProperty("vec2", "float r", asOFFSET(math::vec2, r));
	engine->RegisterObjectProperty("vec2", "float g", asOFFSET(math::vec2, g));
}

void registerVec2Behaviour(asIScriptEngine *engine) {
	engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(const vec2 &in)", asFUNCTION(vec2Copyvec2Constructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(const vec3 &in)", asFUNCTION(vec2Copyvec3Constructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(const vec4 &in)", asFUNCTION(vec2Copyvec4Constructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(vec2InitConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(float = 0)", asFUNCTION(vec2FillConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec2", asBEHAVE_LIST_CONSTRUCT, "void f(const int &in) {float, float}", asFUNCTION(vec2ListConstructor), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectMethod("vec2", "vec2& opAddAssign(const vec2 &in)", asMETHODPR(math::vec2, operator+=, (const math::vec2 &), math::vec2&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "vec2& opSubAssign(const vec2 &in)", asMETHODPR(math::vec2, operator-=, (const math::vec2 &), math::vec2&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "vec2& opMulAssign(const vec2 &in)", asMETHODPR(math::vec2, operator*=, (const math::vec2 &), math::vec2&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "vec2& opDivAssign(const vec2 &in)", asMETHODPR(math::vec2, operator/=, (const math::vec2 &), math::vec2&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "vec2 opAdd(const vec2 &in) const", asMETHODPR(math::vec2, operator+, (const math::vec2 &) const, math::vec2), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "vec2 opSub(const vec2 &in) const", asMETHODPR(math::vec2, operator-, (const math::vec2 &) const, math::vec2), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "vec2 opMul(const vec2 &in) const", asMETHODPR(math::vec2, operator*, (const math::vec2 &) const, math::vec2), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "vec2 opDiv(const vec2 &in) const", asMETHODPR(math::vec2, operator/, (const math::vec2 &) const, math::vec2), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "vec2 opNeg() const", asMETHODPR(math::vec2, operator-, () const, math::vec2), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "bool opEquals(const vec2 &in) const", asMETHODPR(math::vec2, operator==, (const math::vec2 &) const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "vec2& opAssign(const vec2 &in) const", asMETHODPR(math::vec2, operator=, (const math::vec2 &), math::vec2&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec2", "float& opIndex(uint index)", asMETHODPR(math::vec2, operator[], (unsigned), float&), asCALL_THISCALL);
}

void registerVec3(asIScriptEngine *engine) {
	engine->RegisterObjectType("vec3", sizeof(math::vec3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<math::vec3>() | asOBJ_APP_CLASS_ALLFLOATS);
	engine->RegisterObjectProperty("vec3", "float x", asOFFSET(math::vec3, x));
	engine->RegisterObjectProperty("vec3", "float y", asOFFSET(math::vec3, y));
	engine->RegisterObjectProperty("vec3", "float z", asOFFSET(math::vec3, z));
	engine->RegisterObjectProperty("vec3", "vec2 xy", asOFFSET(math::vec3, xy));
	engine->RegisterObjectProperty("vec3", "float r", asOFFSET(math::vec3, r));
	engine->RegisterObjectProperty("vec3", "float g", asOFFSET(math::vec3, g));
	engine->RegisterObjectProperty("vec3", "float b", asOFFSET(math::vec3, b));
}

void registerVec3Behaviour(asIScriptEngine *engine) {
	engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(const vec2 &in, float = 0)", asFUNCTION(vec3Copyvec2floatConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(const vec3 &in)", asFUNCTION(vec3Copyvec3Constructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(const vec4 &in)", asFUNCTION(vec3Copyvec4Constructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float = 0)", asFUNCTION(vec3InitConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float = 0)", asFUNCTION(vec3FillConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec3", asBEHAVE_LIST_CONSTRUCT, "void f(const int &in) {float, float, float}", asFUNCTION(vec3ListConstructor), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectMethod("vec3", "vec3& opAddAssign(const vec3 &in)", asMETHODPR(math::vec3, operator+=, (const math::vec3 &), math::vec3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "vec3& opSubAssign(const vec3 &in)", asMETHODPR(math::vec3, operator-=, (const math::vec3 &), math::vec3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "vec3& opMulAssign(const vec3 &in)", asMETHODPR(math::vec3, operator*=, (const math::vec3 &), math::vec3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "vec3& opDivAssign(const vec3 &in)", asMETHODPR(math::vec3, operator/=, (const math::vec3 &), math::vec3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "vec3 opAdd(const vec3 &in) const", asMETHODPR(math::vec3, operator+, (const math::vec3 &) const, math::vec3), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "vec3 opSub(const vec3 &in) const", asMETHODPR(math::vec3, operator-, (const math::vec3 &) const, math::vec3), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "vec3 opMul(const vec3 &in) const", asMETHODPR(math::vec3, operator*, (const math::vec3 &) const, math::vec3), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "vec3 opDiv(const vec3 &in) const", asMETHODPR(math::vec3, operator/, (const math::vec3 &) const, math::vec3), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "vec3 opNeg() const", asMETHODPR(math::vec3, operator-, () const, math::vec3), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "bool opEquals(const vec3 &in) const", asMETHODPR(math::vec3, operator==, (const math::vec3 &) const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "vec3& opAssign(const vec3 &in) const", asMETHODPR(math::vec3, operator=, (const math::vec3 &), math::vec3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec3", "float& opIndex(uint index)", asMETHODPR(math::vec3, operator[], (unsigned), float&), asCALL_THISCALL);
}

void registerVec4(asIScriptEngine *engine) {
	engine->RegisterObjectType("vec4", sizeof(math::vec4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<math::vec4>() | asOBJ_APP_CLASS_ALLFLOATS);
	engine->RegisterObjectProperty("vec4", "float x", asOFFSET(math::vec4, x));
	engine->RegisterObjectProperty("vec4", "float y", asOFFSET(math::vec4, y));
	engine->RegisterObjectProperty("vec4", "float z", asOFFSET(math::vec4, z));
	engine->RegisterObjectProperty("vec4", "float w", asOFFSET(math::vec4, w));
	engine->RegisterObjectProperty("vec4", "vec2 xy", asOFFSET(math::vec4, xy));
	engine->RegisterObjectProperty("vec4", "vec3 xyz", asOFFSET(math::vec4, xyz));
	engine->RegisterObjectProperty("vec4", "float r", asOFFSET(math::vec4, r));
	engine->RegisterObjectProperty("vec4", "float g", asOFFSET(math::vec4, g));
	engine->RegisterObjectProperty("vec4", "float b", asOFFSET(math::vec4, b));
	engine->RegisterObjectProperty("vec4", "float a", asOFFSET(math::vec4, a));
	engine->RegisterObjectProperty("vec4", "vec3 rgb", asOFFSET(math::vec4, rgb));
}

void registerVec4Behaviour(asIScriptEngine *engine) {
	engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(const vec2 &in, const vec2 &in)", asFUNCTION(vec4Copy2vec2Constructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(const vec2 &in, float = 0, float = 0)", asFUNCTION(vec4Copyvec22floatConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(const vec3 &in, float = 0)", asFUNCTION(vec4Copyvec3floatConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(const vec4 &in)", asFUNCTION(vec4Copyvec4Constructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(float, float, float = 0, float = 0)", asFUNCTION(vec4InitConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(float = 0)", asFUNCTION(vec4FillConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("vec4", asBEHAVE_LIST_CONSTRUCT, "void f(const int &in) {float, float, float, float}", asFUNCTION(vec4ListConstructor), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectMethod("vec4", "vec4& opAddAssign(const vec4 &in)", asMETHODPR(math::vec4, operator+=, (const math::vec4 &), math::vec4&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "vec4& opSubAssign(const vec4 &in)", asMETHODPR(math::vec4, operator-=, (const math::vec4 &), math::vec4&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "vec4& opMulAssign(const vec4 &in)", asMETHODPR(math::vec4, operator*=, (const math::vec4 &), math::vec4&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "vec4& opDivAssign(const vec4 &in)", asMETHODPR(math::vec4, operator/=, (const math::vec4 &), math::vec4&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "vec4 opAdd(const vec4 &in) const", asMETHODPR(math::vec4, operator+, (const math::vec4 &) const, math::vec4), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "vec4 opSub(const vec4 &in) const", asMETHODPR(math::vec4, operator-, (const math::vec4 &) const, math::vec4), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "vec4 opMul(const vec4 &in) const", asMETHODPR(math::vec4, operator*, (const math::vec4 &) const, math::vec4), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "vec4 opDiv(const vec4 &in) const", asMETHODPR(math::vec4, operator/, (const math::vec4 &) const, math::vec4), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "vec4 opNeg() const", asMETHODPR(math::vec4, operator-, () const, math::vec4), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "bool opEquals(const vec4 &in) const", asMETHODPR(math::vec4, operator==, (const math::vec4 &) const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "vec4& opAssign(const vec4 &in) const", asMETHODPR(math::vec4, operator=, (const math::vec4 &), math::vec4&), asCALL_THISCALL);
	engine->RegisterObjectMethod("vec4", "float& opIndex(uint index)", asMETHODPR(math::vec4, operator[], (unsigned), float&), asCALL_THISCALL);
}

void registerVec234HelperFunctions(asIScriptEngine *engine) {
	engine->RegisterGlobalFunction("float length(const vec2 &in)", asFUNCTIONPR(math::length, (const math::vec2&), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("float length(const vec3 &in)", asFUNCTIONPR(math::length, (const math::vec3&), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("float length(const vec4 &in)", asFUNCTIONPR(math::length, (const math::vec4&), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec2 normalize(const vec2 &in)", asFUNCTIONPR(math::normalize, (const math::vec2&), math::vec2), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec3 normalize(const vec3 &in)", asFUNCTIONPR(math::normalize, (const math::vec3&), math::vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec4 normalize(const vec4 &in)", asFUNCTIONPR(math::normalize, (const math::vec4&), math::vec4), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec2 floor(const vec2 &in)", asFUNCTIONPR(math::floor, (const math::vec2&), math::vec2), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec3 floor(const vec3 &in)", asFUNCTIONPR(math::floor, (const math::vec3&), math::vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec4 floor(const vec4 &in)", asFUNCTIONPR(math::floor, (const math::vec4&), math::vec4), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec2 fract(const vec2 &in)", asFUNCTIONPR(math::fract, (const math::vec2&), math::vec2), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec3 fract(const vec3 &in)", asFUNCTIONPR(math::fract, (const math::vec3&), math::vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec4 fract(const vec4 &in)", asFUNCTIONPR(math::fract, (const math::vec4&), math::vec4), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec2 smoothstep(const vec2 &in)", asFUNCTIONPR(math::smoothstep, (const math::vec2&), math::vec2), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec3 smoothstep(const vec3 &in)", asFUNCTIONPR(math::smoothstep, (const math::vec3&), math::vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec4 smoothstep(const vec4 &in)", asFUNCTIONPR(math::smoothstep, (const math::vec4&), math::vec4), asCALL_CDECL);
	engine->RegisterGlobalFunction("float dist(const vec2 &in, const vec2 &in)", asFUNCTIONPR(math::dist, (const math::vec2&, const math::vec2&), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("float dist(const vec3 &in, const vec3 &in)", asFUNCTIONPR(math::dist, (const math::vec3&, const math::vec3&), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("float dist(const vec4 &in, const vec4 &in)", asFUNCTIONPR(math::dist, (const math::vec4&, const math::vec4&), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("float dot(const vec2 &in, const vec2 &in)", asFUNCTIONPR(math::dot, (const math::vec2&, const math::vec2&), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("float dot(const vec3 &in, const vec3 &in)", asFUNCTIONPR(math::dot, (const math::vec3&, const math::vec3&), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("float dot(const vec4 &in, const vec4 &in)", asFUNCTIONPR(math::dot, (const math::vec4&, const math::vec4&), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec3 cross(const vec3 &in, const vec3 &in)", asFUNCTIONPR(math::cross, (const math::vec3&, const math::vec3&), math::vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec2 mix(const vec2 &in, const vec2 &in, float amnt)", asFUNCTIONPR(math::mix, (const math::vec2&, const math::vec2&, float), math::vec2), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec3 mix(const vec3 &in, const vec3 &in, float amnt)", asFUNCTIONPR(math::mix, (const math::vec3&, const math::vec3&, float), math::vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec4 mix(const vec4 &in, const vec4 &in, float amnt)", asFUNCTIONPR(math::mix, (const math::vec4&, const math::vec4&, float), math::vec4), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec2 mix(const vec2 &in, const vec2 &in, const vec2 &in)", asFUNCTIONPR(math::mix, (const math::vec2&, const math::vec2&, const math::vec2&), math::vec2), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec3 mix(const vec3 &in, const vec3 &in, const vec3 &in)", asFUNCTIONPR(math::mix, (const math::vec3&, const math::vec3&, const math::vec3&), math::vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec4 mix(const vec4 &in, const vec4 &in, const vec4 &in)", asFUNCTIONPR(math::mix, (const math::vec4&, const math::vec4&, const math::vec4&), math::vec4), asCALL_CDECL);
}

void registerMat4(asIScriptEngine *engine) {
	engine->RegisterObjectType("mat4", sizeof(math::mat4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<math::mat4>() | asOBJ_APP_CLASS_ALLFLOATS);
	engine->RegisterObjectBehaviour("mat4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(mat4DefaultConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("mat4", asBEHAVE_CONSTRUCT, "void f(float v)", asFUNCTION(mat4FillConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("mat4", asBEHAVE_CONSTRUCT, "void f(const mat4 &in)", asFUNCTION(mat4CopyConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("mat4", asBEHAVE_LIST_CONSTRUCT, "void f(const int &in) {float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float}", asFUNCTION(mat4ListConstructor), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectMethod("mat4", "float& opIndex(uint col, uint row)", asMETHODPR(math::mat4, operator(), (unsigned, unsigned), float&), asCALL_THISCALL);
	engine->RegisterObjectMethod("mat4", "mat4 opMul(const mat4 &in) const", asMETHODPR(math::mat4, operator*, (const math::mat4 &) const, math::mat4), asCALL_THISCALL);
	engine->RegisterObjectMethod("mat4", "mat4 opDiv(const mat4 &in) const", asMETHODPR(math::mat4, operator/, (const math::mat4 &) const, math::mat4), asCALL_THISCALL);
	engine->RegisterObjectMethod("mat4", "vec4 opMul(const vec4 &in) const", asMETHODPR(math::mat4, operator*, (const math::vec4 &) const, math::vec4), asCALL_THISCALL);
	engine->RegisterObjectMethod("mat4", "vec4 opDiv(const vec4 &in) const", asMETHODPR(math::mat4, operator/, (const math::vec4 &) const, math::vec4), asCALL_THISCALL);
	engine->RegisterObjectMethod("mat4", "mat4 inverse() const", asMETHODPR(math::mat4, inverse, () const, math::mat4), asCALL_THISCALL);
	engine->RegisterGlobalFunction("mat4 translation(const vec3 &in)", asFUNCTIONPR(math::mat4::translation, (const math::vec3&), math::mat4), asCALL_CDECL);
	engine->RegisterGlobalFunction("mat4 rotation(const vec3 &in)", asFUNCTIONPR(math::mat4::rotation, (const math::vec3&), math::mat4), asCALL_CDECL);
	engine->RegisterGlobalFunction("mat4 scaling(const vec3 &in)", asFUNCTIONPR(math::mat4::scaling, (const math::vec3&), math::mat4), asCALL_CDECL);
	engine->RegisterGlobalFunction("mat4 shearing(float, float, float, float, float, float)", asFUNCTIONPR(math::mat4::shearing, (float, float, float, float, float, float), math::mat4), asCALL_CDECL);
	engine->RegisterObjectMethod("mat4", "mat4 translate(const vec3 &in)", asMETHODPR(math::mat4, translate, (const math::vec3 &) const, math::mat4), asCALL_THISCALL);
	engine->RegisterObjectMethod("mat4", "mat4 rotate(const vec3 &in)", asMETHODPR(math::mat4, rotate, (const math::vec3 &) const, math::mat4), asCALL_THISCALL);
	engine->RegisterObjectMethod("mat4", "mat4 scale(const vec3 &in)", asMETHODPR(math::mat4, scale, (const math::vec3 &) const, math::mat4), asCALL_THISCALL);
	engine->RegisterObjectMethod("mat4", "mat4 shear(float, float, float, float, float, float)", asMETHODPR(math::mat4, shear, (float, float, float, float, float, float) const, math::mat4), asCALL_THISCALL);
	engine->RegisterGlobalFunction("mat4 projection(float fov, float aspect, float znear, float zfar)", asFUNCTIONPR(math::mat4::projection, (float, float, float, float), math::mat4), asCALL_CDECL);
}

void registerMathHelperFunctions(asIScriptEngine *engine) {
	engine->RegisterGlobalFunction("float rand(float min = 0.0, float max = 1.0)", asFUNCTIONPR(
		[](float min, float max) -> float { float r = float(rand()) / float(RAND_MAX); r *= (max - min); r += min; return r; }
		, (float, float), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("float min(float, float)", asFUNCTIONPR(math::min, (float, float), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("float max(float, float)", asFUNCTIONPR(math::max, (float, float), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("float clamp(float, float, float)", asFUNCTIONPR(math::clamp, (float, float, float), float), asCALL_CDECL);
}

void registerImGui(asIScriptEngine *engine) {
	engine->SetDefaultNamespace("imgui");

	engine->RegisterEnum("ConfigFlags");
	engine->RegisterEnumValue("ConfigFlags", "None",					ImGuiConfigFlags_None);
	engine->RegisterEnumValue("ConfigFlags", "NavEnableKeyboard",		ImGuiConfigFlags_NavEnableKeyboard);
	engine->RegisterEnumValue("ConfigFlags", "NavEnableGamepad",		ImGuiConfigFlags_NavEnableGamepad);
	engine->RegisterEnumValue("ConfigFlags", "NavEnableSetMousePos",	ImGuiConfigFlags_NavEnableSetMousePos);
	engine->RegisterEnumValue("ConfigFlags", "NavNoCaptureKeyboard",	ImGuiConfigFlags_NavNoCaptureKeyboard);
	engine->RegisterEnumValue("ConfigFlags", "NoMouse",					ImGuiConfigFlags_NoMouse);
	engine->RegisterEnumValue("ConfigFlags", "NoMouseCursorChange",		ImGuiConfigFlags_NoMouseCursorChange);
	engine->RegisterEnumValue("ConfigFlags", "DockingEnable",			ImGuiConfigFlags_DockingEnable);
	engine->RegisterEnumValue("ConfigFlags", "ViewportsEnable",			ImGuiConfigFlags_ViewportsEnable);
	engine->RegisterEnumValue("ConfigFlags", "DpiEnableScaleViewports",	ImGuiConfigFlags_DpiEnableScaleViewports);
	engine->RegisterEnumValue("ConfigFlags", "DpiEnableScaleFonts",		ImGuiConfigFlags_DpiEnableScaleFonts);
	engine->RegisterEnumValue("ConfigFlags", "IsSRGB",					ImGuiConfigFlags_IsSRGB);
	engine->RegisterEnumValue("ConfigFlags", "IsTouchScreen",			ImGuiConfigFlags_IsTouchScreen);

	engine->RegisterEnum("BackendFlags");
	engine->RegisterEnumValue("BackendFlags", "None",						ImGuiBackendFlags_None);
	engine->RegisterEnumValue("BackendFlags", "HasGamepad",					ImGuiBackendFlags_HasGamepad);
	engine->RegisterEnumValue("BackendFlags", "HasMouseCursors",			ImGuiBackendFlags_HasMouseCursors);
	engine->RegisterEnumValue("BackendFlags", "HasSetMousePos",				ImGuiBackendFlags_HasSetMousePos);
	engine->RegisterEnumValue("BackendFlags", "RendererHasVtxOffset",		ImGuiBackendFlags_RendererHasVtxOffset);
	engine->RegisterEnumValue("BackendFlags", "PlatformHasViewports",		ImGuiBackendFlags_PlatformHasViewports);
	engine->RegisterEnumValue("BackendFlags", "HasMouseHoveredViewport",	ImGuiBackendFlags_HasMouseHoveredViewport);
	engine->RegisterEnumValue("BackendFlags", "RendererHasViewports",		ImGuiBackendFlags_RendererHasViewports);

	engine->RegisterEnum("WindowFlags");
	engine->RegisterEnumValue("WindowFlags", "None",						ImGuiWindowFlags_None);
	engine->RegisterEnumValue("WindowFlags", "NoTitleBar",					ImGuiWindowFlags_NoTitleBar);
	engine->RegisterEnumValue("WindowFlags", "NoResize",					ImGuiWindowFlags_NoResize);
	engine->RegisterEnumValue("WindowFlags", "NoMove",						ImGuiWindowFlags_NoMove);
	engine->RegisterEnumValue("WindowFlags", "NoScrollbar",					ImGuiWindowFlags_NoScrollbar);
	engine->RegisterEnumValue("WindowFlags", "NoScrollWithMouse",			ImGuiWindowFlags_NoScrollWithMouse);
	engine->RegisterEnumValue("WindowFlags", "NoCollapse",					ImGuiWindowFlags_NoCollapse);
	engine->RegisterEnumValue("WindowFlags", "AlwaysAutoResize",			ImGuiWindowFlags_AlwaysAutoResize);
	engine->RegisterEnumValue("WindowFlags", "NoBackground",				ImGuiWindowFlags_NoBackground);
	engine->RegisterEnumValue("WindowFlags", "NoSavedSettings",				ImGuiWindowFlags_NoSavedSettings);
	engine->RegisterEnumValue("WindowFlags", "NoMouseInputs",				ImGuiWindowFlags_NoMouseInputs);
	engine->RegisterEnumValue("WindowFlags", "MenuBar",						ImGuiWindowFlags_MenuBar);
	engine->RegisterEnumValue("WindowFlags", "HorizontalScrollbar",			ImGuiWindowFlags_HorizontalScrollbar);
	engine->RegisterEnumValue("WindowFlags", "NoFocusOnAppearing",			ImGuiWindowFlags_NoFocusOnAppearing);
	engine->RegisterEnumValue("WindowFlags", "NoBringToFrontOnFocus",		ImGuiWindowFlags_NoBringToFrontOnFocus);
	engine->RegisterEnumValue("WindowFlags", "AlwaysVerticalScrollbar",		ImGuiWindowFlags_AlwaysVerticalScrollbar);
	engine->RegisterEnumValue("WindowFlags", "AlwaysHorizontalScrollbar",	ImGuiWindowFlags_AlwaysHorizontalScrollbar);
	engine->RegisterEnumValue("WindowFlags", "AlwaysUseWindowPadding",		ImGuiWindowFlags_AlwaysUseWindowPadding);
	engine->RegisterEnumValue("WindowFlags", "NoNavInputs",					ImGuiWindowFlags_NoNavInputs);
	engine->RegisterEnumValue("WindowFlags", "NoNavFocus",					ImGuiWindowFlags_NoNavFocus);
	engine->RegisterEnumValue("WindowFlags", "UnsavedDocument",				ImGuiWindowFlags_UnsavedDocument);
	engine->RegisterEnumValue("WindowFlags", "NoDocking",					ImGuiWindowFlags_NoDocking);
	engine->RegisterEnumValue("WindowFlags", "NoNav",						ImGuiWindowFlags_NoNav);
	engine->RegisterEnumValue("WindowFlags", "NoDecoration",				ImGuiWindowFlags_NoDecoration);
	engine->RegisterEnumValue("WindowFlags", "NoInputs",					ImGuiWindowFlags_NoInputs);

	engine->RegisterEnum("Dir");
	engine->RegisterEnumValue("Dir", "None",	ImGuiDir_None);
	engine->RegisterEnumValue("Dir", "Left",	ImGuiDir_Left);
	engine->RegisterEnumValue("Dir", "Right",	ImGuiDir_Right);
	engine->RegisterEnumValue("Dir", "Up",		ImGuiDir_Up);
	engine->RegisterEnumValue("Dir", "Down",	ImGuiDir_Down);

	engine->RegisterEnum("Color");
	engine->RegisterEnumValue("Color", "Text",					ImGuiCol_Text);
	engine->RegisterEnumValue("Color", "TextDisabled",			ImGuiCol_TextDisabled);
	engine->RegisterEnumValue("Color", "WindowBg",				ImGuiCol_WindowBg);
	engine->RegisterEnumValue("Color", "ChildBg",				ImGuiCol_ChildBg);
	engine->RegisterEnumValue("Color", "PopupBg",				ImGuiCol_PopupBg);
	engine->RegisterEnumValue("Color", "Border",				ImGuiCol_Border);
	engine->RegisterEnumValue("Color", "BorderShadow",			ImGuiCol_BorderShadow);
	engine->RegisterEnumValue("Color", "FrameBg",				ImGuiCol_FrameBg);
	engine->RegisterEnumValue("Color", "FrameBgHovered",		ImGuiCol_FrameBgHovered);
	engine->RegisterEnumValue("Color", "FrameBgActive",			ImGuiCol_FrameBgActive);
	engine->RegisterEnumValue("Color", "TitleBg",				ImGuiCol_TitleBg);
	engine->RegisterEnumValue("Color", "TitleBgActive",			ImGuiCol_TitleBgActive);
	engine->RegisterEnumValue("Color", "TitleBgCollapsed",		ImGuiCol_TitleBgCollapsed);
	engine->RegisterEnumValue("Color", "MenuBarBg",				ImGuiCol_MenuBarBg);
	engine->RegisterEnumValue("Color", "ScrollbarBg",			ImGuiCol_ScrollbarBg);
	engine->RegisterEnumValue("Color", "ScrollbarGrab",			ImGuiCol_ScrollbarGrab);
	engine->RegisterEnumValue("Color", "ScrollbarGrabHovered",	ImGuiCol_ScrollbarGrabHovered);
	engine->RegisterEnumValue("Color", "ScrollbarGrabActive",	ImGuiCol_ScrollbarGrabActive);
	engine->RegisterEnumValue("Color", "CheckMark",				ImGuiCol_CheckMark);
	engine->RegisterEnumValue("Color", "SliderGrab",			ImGuiCol_SliderGrab);
	engine->RegisterEnumValue("Color", "SliderGrabActive",		ImGuiCol_SliderGrabActive);
	engine->RegisterEnumValue("Color", "Button",				ImGuiCol_Button);
	engine->RegisterEnumValue("Color", "ButtonHovered",			ImGuiCol_ButtonHovered);
	engine->RegisterEnumValue("Color", "ButtonActive",			ImGuiCol_ButtonActive);
	engine->RegisterEnumValue("Color", "Header",				ImGuiCol_Header);
	engine->RegisterEnumValue("Color", "HeaderHovered",			ImGuiCol_HeaderHovered);
	engine->RegisterEnumValue("Color", "HeaderActive",			ImGuiCol_HeaderActive);
	engine->RegisterEnumValue("Color", "Separator",				ImGuiCol_Separator);
	engine->RegisterEnumValue("Color", "SeparatorHovered",		ImGuiCol_SeparatorHovered);
	engine->RegisterEnumValue("Color", "SeparatorActive",		ImGuiCol_SeparatorActive);
	engine->RegisterEnumValue("Color", "ResizeGrip",			ImGuiCol_ResizeGrip);
	engine->RegisterEnumValue("Color", "ResizeGripHovered",		ImGuiCol_ResizeGripHovered);
	engine->RegisterEnumValue("Color", "ResizeGripActive",		ImGuiCol_ResizeGripActive);
	engine->RegisterEnumValue("Color", "Tab",					ImGuiCol_Tab);
	engine->RegisterEnumValue("Color", "TabHovered",			ImGuiCol_TabHovered);
	engine->RegisterEnumValue("Color", "TabActive",				ImGuiCol_TabActive);
	engine->RegisterEnumValue("Color", "TabUnfocused",			ImGuiCol_TabUnfocused);
	engine->RegisterEnumValue("Color", "TabUnfocusedActive",	ImGuiCol_TabUnfocusedActive);
	engine->RegisterEnumValue("Color", "DockingPreview",		ImGuiCol_DockingPreview);
	engine->RegisterEnumValue("Color", "DockingEmptyBg",		ImGuiCol_DockingEmptyBg);
	engine->RegisterEnumValue("Color", "PlotLInes",				ImGuiCol_PlotLines);
	engine->RegisterEnumValue("Color", "PlotLinesHovered",		ImGuiCol_PlotLinesHovered);
	engine->RegisterEnumValue("Color", "PlotHistogram",			ImGuiCol_PlotHistogram);
	engine->RegisterEnumValue("Color", "PlotHistogramHovered",	ImGuiCol_PlotHistogramHovered);
	engine->RegisterEnumValue("Color", "TextSelectedBg",		ImGuiCol_TextSelectedBg);
	engine->RegisterEnumValue("Color", "DragDropTarget",		ImGuiCol_DragDropTarget);
	engine->RegisterEnumValue("Color", "NavHighlight",			ImGuiCol_NavHighlight);
	engine->RegisterEnumValue("Color", "NavWindowHighlight",	ImGuiCol_NavWindowingHighlight);
	engine->RegisterEnumValue("Color", "NavWindowDimBg",		ImGuiCol_NavWindowingDimBg);
	engine->RegisterEnumValue("Color", "ModalWindowDimBg",		ImGuiCol_ModalWindowDimBg);

	engine->RegisterEnum("StyleVar");
	engine->RegisterEnumValue("StyleVar", "Alpha",					ImGuiStyleVar_Alpha);
	engine->RegisterEnumValue("StyleVar", "WindowPadding",			ImGuiStyleVar_WindowPadding);
	engine->RegisterEnumValue("StyleVar", "WindowRounding",			ImGuiStyleVar_WindowRounding);
	engine->RegisterEnumValue("StyleVar", "WindowBorderSize",		ImGuiStyleVar_WindowBorderSize);
	engine->RegisterEnumValue("StyleVar", "WindowMinSIze",			ImGuiStyleVar_WindowMinSize);
	engine->RegisterEnumValue("StyleVar", "WIndowTitleAlign",		ImGuiStyleVar_WindowTitleAlign);
	engine->RegisterEnumValue("StyleVar", "ChildRounding",			ImGuiStyleVar_ChildRounding);
	engine->RegisterEnumValue("StyleVar", "ChildBorderSize",		ImGuiStyleVar_ChildBorderSize);
	engine->RegisterEnumValue("StyleVar", "PopupRounding",			ImGuiStyleVar_PopupRounding);
	engine->RegisterEnumValue("StyleVar", "PopupBorderSize",		ImGuiStyleVar_PopupBorderSize);
	engine->RegisterEnumValue("StyleVar", "FramePadding",			ImGuiStyleVar_FramePadding);
	engine->RegisterEnumValue("StyleVar", "FrameRounding",			ImGuiStyleVar_FrameRounding);
	engine->RegisterEnumValue("StyleVar", "FrameBorderSize",		ImGuiStyleVar_FrameBorderSize);
	engine->RegisterEnumValue("StyleVar", "ItemSpacing",			ImGuiStyleVar_ItemSpacing);
	engine->RegisterEnumValue("StyleVar", "ItemInnerSpacing",		ImGuiStyleVar_ItemInnerSpacing);
	engine->RegisterEnumValue("StyleVar", "IndentSpacing",			ImGuiStyleVar_IndentSpacing);
	engine->RegisterEnumValue("StyleVar", "ScrollbarSize",			ImGuiStyleVar_ScrollbarSize);
	engine->RegisterEnumValue("StyleVar", "ScrollbarRounding",		ImGuiStyleVar_ScrollbarRounding);
	engine->RegisterEnumValue("StyleVar", "GrabMinSize",			ImGuiStyleVar_GrabMinSize);
	engine->RegisterEnumValue("StyleVar", "GrabRounding",			ImGuiStyleVar_GrabRounding);
	engine->RegisterEnumValue("StyleVar", "TabRounding",			ImGuiStyleVar_TabRounding);
	engine->RegisterEnumValue("StyleVar", "ButtonTextAlign",		ImGuiStyleVar_ButtonTextAlign);
	engine->RegisterEnumValue("StyleVar", "SelectableTextAlign",	ImGuiStyleVar_SelectableTextAlign);

	engine->RegisterEnum("DockNodeFlags");
	engine->RegisterEnumValue("DockNodeFlags", "None",						ImGuiDockNodeFlags_None);
	engine->RegisterEnumValue("DockNodeFlags", "KeepAliveOnly",				ImGuiDockNodeFlags_KeepAliveOnly);
	engine->RegisterEnumValue("DockNodeFlags", "NoDockingInCentralNode",	ImGuiDockNodeFlags_NoDockingInCentralNode);
	engine->RegisterEnumValue("DockNodeFlags", "PassthruCentralNode",		ImGuiDockNodeFlags_PassthruCentralNode);
	engine->RegisterEnumValue("DockNodeFlags", "NoSplit",					ImGuiDockNodeFlags_NoSplit);
	engine->RegisterEnumValue("DockNodeFlags", "NoResize",					ImGuiDockNodeFlags_NoResize);
	engine->RegisterEnumValue("DockNodeFlags", "AutoHideTabBar",			ImGuiDockNodeFlags_AutoHideTabBar);

	engine->RegisterEnum("ComboFlags");
	engine->RegisterEnumValue("ComboFlags", "None",				ImGuiComboFlags_None);
	engine->RegisterEnumValue("ComboFlags", "PopupAlignLeft",	ImGuiComboFlags_PopupAlignLeft);
	engine->RegisterEnumValue("ComboFlags", "HeightSmall",		ImGuiComboFlags_HeightSmall);
	engine->RegisterEnumValue("ComboFlags", "HeightRegular",	ImGuiComboFlags_HeightRegular);
	engine->RegisterEnumValue("ComboFlags", "HeightLarge",		ImGuiComboFlags_HeightLarge);
	engine->RegisterEnumValue("ComboFlags", "HeightLargest",	ImGuiComboFlags_HeightLargest);
	engine->RegisterEnumValue("ComboFlags", "NoArrowButton",	ImGuiComboFlags_NoArrowButton);
	engine->RegisterEnumValue("ComboFlags", "NoPreview",		ImGuiComboFlags_NoPreview);

	engine->RegisterObjectType("ImGuiIO", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectProperty("ImGuiIO", "ConfigFlags configFlags", asOFFSET(ImGuiIO, ConfigFlags));
	engine->RegisterObjectProperty("ImGuiIO", "BackendFlags backendFlags", asOFFSET(ImGuiIO, BackendFlags));
	engine->RegisterObjectProperty("ImGuiIO", "vec2 displaySize", asOFFSET(ImGuiIO, DisplaySize));
	engine->RegisterObjectProperty("ImGuiIO", "float dt", asOFFSET(ImGuiIO, DeltaTime));
	engine->RegisterObjectProperty("ImGuiIO", "float iniSavingRate", asOFFSET(ImGuiIO, IniSavingRate));
	engine->RegisterGlobalProperty("ImGuiIO io", &(ImGui::GetIO()));
	engine->RegisterObjectType("ImGuiStyle", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectProperty("ImGuiStyle", "float alpha", asOFFSET(ImGuiStyle, Alpha));
	engine->RegisterObjectProperty("ImGuiStyle", "vec2 windowPadding", asOFFSET(ImGuiStyle, WindowPadding));
	engine->RegisterObjectProperty("ImGuiStyle", "float windowRounding", asOFFSET(ImGuiStyle, WindowRounding));
	engine->RegisterObjectProperty("ImGuiStyle", "int windowMenuButtonPosition", asOFFSET(ImGuiStyle, WindowMenuButtonPosition));
	engine->RegisterGlobalProperty("ImGuiStyle style", &(ImGui::GetStyle()));

	engine->RegisterGlobalFunction("uint getID(string name)", asFUNCTIONPR([](std::string name) -> unsigned { return ImGui::GetID(name.c_str()); }, (std::string), unsigned), asCALL_CDECL);
	engine->RegisterGlobalFunction("void dockSpace(uint id, vec2 size = vec2(0), int flags = 0)", asFUNCTIONPR([](unsigned id, math::vec2 size, int flags) { ImGui::DockSpace(id, ImVec2(size.x, size.y), flags); }, (unsigned, math::vec2, int), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void showDemoWindow(bool &out)", asFUNCTION(ImGui::ShowDemoWindow), asCALL_CDECL);
	engine->RegisterGlobalFunction("void showDemoWindow()", asFUNCTIONPR([](){ImGui::ShowDemoWindow(nullptr);}, (), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void showAboutWindow(bool &out)", asFUNCTION(ImGui::ShowAboutWindow), asCALL_CDECL);
	engine->RegisterGlobalFunction("void showAboutWindow()", asFUNCTIONPR([](){ImGui::ShowAboutWindow(nullptr);}, (), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void showMetricsWindow(bool &out)", asFUNCTION(ImGui::ShowMetricsWindow), asCALL_CDECL);
	engine->RegisterGlobalFunction("void showMetricsWindow()", asFUNCTIONPR([](){ImGui::ShowMetricsWindow(nullptr);}, (), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void pushStyleColor(int idx, vec4 &in)", asFUNCTIONPR(ImGui::PushStyleColor, (int, const ImVec4&), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void pushStyleVar(int idx, float val)", asFUNCTIONPR(ImGui::PushStyleVar, (int, float), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void pushStyleVar(int idx, vec2 &in)", asFUNCTIONPR(ImGui::PushStyleVar, (int, const ImVec2&), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void popStyleColor(int cout = 1)", asFUNCTION(ImGui::PopStyleColor), asCALL_CDECL);
	engine->RegisterGlobalFunction("void popStyleVar(int cout = 1)", asFUNCTION(ImGui::PopStyleVar), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint loadFont(string filename, float size = 12.0f)", asFUNCTIONPR([](std::string filename, float size) -> ImFont* { return ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), size); }, (std::string, float), ImFont*), asCALL_CDECL);
	engine->RegisterGlobalFunction("void pushFont(uint f)", asFUNCTION(ImGui::PushFont), asCALL_CDECL);
	engine->RegisterGlobalFunction("void popFont()", asFUNCTION(ImGui::PopFont), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec2 getWindowPos()", asFUNCTION(ImGui::GetWindowPos), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec2 getWindowSize()", asFUNCTION(ImGui::GetWindowSize), asCALL_CDECL);
	engine->RegisterGlobalFunction("void setNextWindowPos(vec2 pos, bool cond = false, vec2 pivot = vec2(0))", asFUNCTION(ImGui::SetNextWindowPos), asCALL_CDECL);
	engine->RegisterGlobalFunction("void setNextWindowSize(vec2 size, bool cond = false)", asFUNCTION(ImGui::SetNextWindowSize), asCALL_CDECL);
	engine->RegisterGlobalFunction("void setNextWindowContentSize(vec2 size)", asFUNCTION(ImGui::SetNextWindowContentSize), asCALL_CDECL);
	engine->RegisterGlobalFunction("void setNextWindowFocus()", asFUNCTION(ImGui::SetNextWindowFocus), asCALL_CDECL);
	engine->RegisterGlobalFunction("void setNextWindowBgAlpha(float alpha)", asFUNCTION(ImGui::SetNextWindowBgAlpha), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool begin(string title, bool &out, int flags = WindowFlags::None)", asFUNCTIONPR(
		[](std::string title, bool *p_open, ImGuiWindowFlags flags) -> bool { return ImGui::Begin(title.c_str(), p_open, flags); }, (std::string, bool*, ImGuiWindowFlags), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool begin(string title, int flags = WindowFlags::None)", asFUNCTIONPR(
		[](std::string title, ImGuiWindowFlags flags) -> bool { return ImGui::Begin(title.c_str(), nullptr, flags); }, (std::string, ImGuiWindowFlags), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("void end()", asFUNCTIONPR(ImGui::End, (void), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void separator()", asFUNCTION(ImGui::Separator), asCALL_CDECL);
	engine->RegisterGlobalFunction("void sameLine(float offsetx = 0.0f, float spacing = -1.0f)", asFUNCTION(ImGui::SameLine), asCALL_CDECL);
	engine->RegisterGlobalFunction("void newLine()", asFUNCTION(ImGui::NewLine), asCALL_CDECL);
	engine->RegisterGlobalFunction("void spacing()", asFUNCTION(ImGui::Spacing), asCALL_CDECL);
	engine->RegisterGlobalFunction("void dummy(vec2 &in)", asFUNCTION(ImGui::Dummy), asCALL_CDECL);
	engine->RegisterGlobalFunction("void indent(float amnt = 0.0f)", asFUNCTION(ImGui::Indent), asCALL_CDECL);
	engine->RegisterGlobalFunction("void unindent(float amnt = 0.0f)", asFUNCTION(ImGui::Unindent), asCALL_CDECL);
	engine->RegisterGlobalFunction("void beginGroup()", asFUNCTION(ImGui::BeginGroup), asCALL_CDECL);
	engine->RegisterGlobalFunction("void endGroup()", asFUNCTION(ImGui::EndGroup), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec2 getCursorPos()", asFUNCTION(ImGui::GetCursorPos), asCALL_CDECL);
	engine->RegisterGlobalFunction("void setCursorPos(vec2 &in)", asFUNCTION(ImGui::SetCursorPos), asCALL_CDECL);
	engine->RegisterGlobalFunction("vec2 getCursorScreenPos()", asFUNCTION(ImGui::GetCursorScreenPos), asCALL_CDECL);
	engine->RegisterGlobalFunction("void setCursorScreenPos(vec2 &in)", asFUNCTION(ImGui::SetCursorScreenPos), asCALL_CDECL);
	engine->RegisterGlobalFunction("void text(string text)", asFUNCTIONPR([](std::string str){ ImGui::Text(str.c_str()); }, (std::string), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void textColored(string text, vec4 color)", asFUNCTIONPR([](std::string str, math::vec4 color){ ImGui::TextColored(ImVec4(color.r, color.g, color.b, color.a), str.c_str()); }, (std::string, math::vec4), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void textDisabled(string text)", asFUNCTIONPR([](std::string str){ ImGui::TextDisabled(str.c_str()); }, (std::string), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void textWrapped(string text)", asFUNCTIONPR([](std::string str){ ImGui::TextWrapped(str.c_str()); }, (std::string), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void labelText(string label, string text)", asFUNCTIONPR([](std::string label, std::string str){ ImGui::LabelText(label.c_str(), str.c_str()); }, (std::string, std::string), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void bulletText(string text)", asFUNCTIONPR([](std::string str){ ImGui::BulletText(str.c_str()); }, (std::string), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool button(string label, vec2 size = vec2(0))", asFUNCTIONPR([](std::string label, math::vec2 size) -> bool { return ImGui::Button(label.c_str(), ImVec2(size.x, size.y)); }, (std::string, math::vec2), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool smallButton(string label)", asFUNCTIONPR([](std::string label) -> bool { return ImGui::SmallButton(label.c_str()); }, (std::string), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool invisibleButton(string label, vec2 size = vec2(0))", asFUNCTIONPR([](std::string label, math::vec2 size) -> bool { return ImGui::InvisibleButton(label.c_str(), ImVec2(size.x, size.y)); }, (std::string, math::vec2), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool arrowButton(string label, Dir dir)", asFUNCTIONPR([](std::string label, ImGuiDir dir) -> bool { return ImGui::ArrowButton(label.c_str(), dir); }, (std::string, ImGuiDir), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool checkbox(string label, bool active)", asFUNCTIONPR([](std::string label, bool active) -> bool { ImGui::Checkbox(label.c_str(), &active); return active; }, (std::string, bool), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool radioButton(string label, bool active)", asFUNCTIONPR([](std::string label, bool active) -> bool { return ImGui::RadioButton(label.c_str(), active); }, (std::string, bool), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("void progressBar(float amnt, vec2 size = vec2(-1,0), string label = \"\")", asFUNCTIONPR([](float amnt, math::vec2 size, std::string label) -> void { ImGui::ProgressBar(amnt, ImVec2(size.x, size.y), label.c_str()); }, (float, math::vec2, std::string), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void bullet()", asFUNCTION(ImGui::Bullet), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool selectable(string name, bool selected, int flags = 0, vec2 size = vec2(0))", asFUNCTIONPR([](std::string label, bool selected, int flags, math::vec2 size) -> bool { return ImGui::Selectable(label.c_str(), selected, flags, ImVec2(size.x, size.y)); }, (std::string, bool, int, math::vec2), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool beginCombo(string label, string preview, int flags = 0)", asFUNCTIONPR([](std::string label, std::string preview, int flags) -> bool { return ImGui::BeginCombo(label.c_str(), preview.c_str(), flags);}, (std::string, std::string, int), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("void endCombo()", asFUNCTION(ImGui::EndCombo), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool beginMenuBar()", asFUNCTION(ImGui::BeginMenuBar), asCALL_CDECL);
	engine->RegisterGlobalFunction("void endMenuBar()", asFUNCTION(ImGui::EndMenuBar), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool beginMainMenuBar()", asFUNCTION(ImGui::BeginMainMenuBar), asCALL_CDECL);
	engine->RegisterGlobalFunction("void endMainMenuBar()", asFUNCTION(ImGui::EndMainMenuBar), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool beginMenu(string label, bool enabled = true)", asFUNCTIONPR([](std::string label, bool enabled) -> bool { return ImGui::BeginMenu(label.c_str(), enabled); }, (std::string, bool), bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("void endMenu()", asFUNCTION(ImGui::EndMenu), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool menuItem(string label, string shortcut = \"\", bool selected = false, bool enabled = true)", asFUNCTIONPR([](std::string label, std::string shortcut, bool selected, bool enabled) -> bool { return ImGui::MenuItem(label.c_str(), shortcut.c_str(), selected, enabled); }, (std::string, std::string, bool, bool), bool), asCALL_CDECL);

	engine->SetDefaultNamespace("");
}

void Photon::initAngelscript() {
	engine = asCreateScriptEngine();
	engine->SetMessageCallback(asMETHODPR(Photon, scriptMessageCallback, (const asSMessageInfo *msg), void), this, asCALL_THISCALL);

	RegisterScriptAny(engine);
	RegisterScriptMath(engine);
	RegisterScriptArray(engine, true);
	RegisterStdString(engine);
	RegisterStdStringUtils(engine);
	RegisterScriptDateTime(engine);
	RegisterScriptDictionary(engine);
	RegisterScriptFile(engine);
	RegisterScriptFileSystem(engine);
	RegisterScriptGrid(engine);
	RegisterScriptHandle(engine);
	RegisterExceptionRoutines(engine);
	RegisterScriptMathComplex(engine);
	RegisterScriptWeakRef(engine);

	registerVec2(engine);
	registerVec3(engine);
	registerVec4(engine);
	registerVec2Behaviour(engine);
	registerVec3Behaviour(engine);
	registerVec4Behaviour(engine);
	registerVec234HelperFunctions(engine);
	registerMat4(engine);
	registerMathHelperFunctions(engine);
	registerImGui(engine);

	engine->RegisterFuncdef("void thread()");
	
	engine->RegisterObjectType("Vertex", sizeof(Vertex), asOBJ_VALUE | asOBJ_POD);
	engine->RegisterObjectProperty("Vertex", "vec3 position", 0);
	engine->RegisterObjectProperty("Vertex", "vec3 normal", sizeof(math::vec3));
	engine->RegisterObjectProperty("Vertex", "vec2 texcoord", sizeof(math::vec3) * 2);

	engine->RegisterObjectType("Mesh", sizeof(photonvk::Mesh*), asOBJ_VALUE | asOBJ_POD);
	engine->RegisterObjectMethod("Mesh", "mat4 get_transform() property", asFUNCTIONPR([](photonvk::Mesh *that) -> math::mat4 { return that->transform; }, (photonvk::Mesh*), math::mat4), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Mesh", "void set_transform(mat4 transform) property", asFUNCTIONPR([](photonvk::Mesh *that, math::mat4 transform) { that->transform = transform; }, (photonvk::Mesh*, math::mat4), void), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectType("Photon", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectMethod("Photon", "void createThread(thread@)", 	asMETHODPR(Photon, createScriptThread, (asIScriptFunction*), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void print(string val)", 		asMETHODPR(Photon, scriptPrint, (std::string), void), 	asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void print(int val)", 			asMETHODPR(Photon, scriptPrint, (int32_t), void), 			asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void print(int64 val)", 		asMETHODPR(Photon, scriptPrint, (int64_t), void), 			asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void print(uint val)", 			asMETHODPR(Photon, scriptPrint, (uint32_t), void), 			asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void print(uint64 val)", 		asMETHODPR(Photon, scriptPrint, (uint64_t), void), 			asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void print(double val)", 		asMETHODPR(Photon, scriptPrint, (double), void), 		asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void print(vec2 val)", 			asMETHODPR(Photon, scriptPrint, (math::vec2), void), 	asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void print(vec3 val)", 			asMETHODPR(Photon, scriptPrint, (math::vec3), void), 	asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void print(vec4 val)", 			asMETHODPR(Photon, scriptPrint, (math::vec4), void), 	asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void print(mat4 &in)", 			asMETHODPR(Photon, scriptPrint, (math::mat4&), void), 	asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "float input(string name)", 		asMETHODPR(Photon, getInput, 	(std::string), float), 	asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "void loadScript(string name, string module, bool runmain)", asMETHODPR(Photon, loadScript, (std::string, std::string, bool), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "double time()", asFUNCTION(glfwGetTime), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Photon", "vec2 getFramebufferSize()", asFUNCTIONPR([](Photon *photon) -> math::vec2 { vk::Extent2D extent = photon->swapchain->getExtent(); return math::vec2(extent.width, extent.height); }, (Photon*), math::vec2), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Photon", "void setMosueVisible(bool val)",asMETHODPR(Photon, setMouseVisible, (bool), void), 			asCALL_THISCALL);
	engine->RegisterObjectMethod("Photon", "bool getMosueVisible()",		asMETHODPR(Photon, getMouseVisible, (), bool), 			asCALL_THISCALL);
	//engine->RegisterObjectMethod("Photon", "Mesh createMesh(array<Vertex> &in)",	asMETHODPR(Photon, createMesh, (CScriptArray&), std::shared_ptr<photonvk::Mesh>), asCALL_THISCALL);
	//engine->RegisterObjectMethod("Photon", "Mesh createMesh(array<Vertex> &in, array<uint16> &in)",	asMETHODPR(Photon, createMeshIndexed, (CScriptArray&, CScriptArray&), std::shared_ptr<photonvk::Mesh>), asCALL_THISCALL);
	//engine->RegisterObjectMethod("Photon", "void addMeshToScene(Mesh m)", asMETHOD(Photon, addMeshToScene), asCALL_THISCALL);
	//engine->RegisterObjectMethod("Photon", "void removeMeshFromScene(Mesh m)", asMETHOD(Photon, removeMeshFromScene), asCALL_THISCALL);
	engine->RegisterGlobalProperty("Photon photon", this);

	contextManager.SetGetTimeCallback([]() -> asUINT {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	});
	contextManager.RegisterThreadSupport(engine);
	contextManager.RegisterCoRoutineSupport(engine);

	builder.SetIncludeCallback([](const char *include, const char *from, CScriptBuilder *builder, void*) -> int {
		return builder->AddSectionFromFile(include);
	}, nullptr);
}

void Photon::loadScript(std::string file, std::string module, bool runmain) {
	builder.StartNewModule(engine, module.c_str());
	builder.AddSectionFromFile(file.c_str());
	builder.BuildModule();
	if(runmain) {
		asIScriptModule *mod = engine->GetModule(module.c_str(), asGM_ONLY_IF_EXISTS); assert(mod);
		asIScriptFunction *func = mod->GetFunctionByName("main");
		if(func) {
			contextManager.AddContext(engine, func);
		}
	}
}

void Photon::createScriptThread(asIScriptFunction *func) {
	asIScriptContext *ctx = asGetActiveContext();
	if(ctx) {
		CContextMgr *ctxMgr = reinterpret_cast<CContextMgr*>(ctx->GetUserData(1002));
		if(ctxMgr) {
			ctxMgr->AddContext(ctx->GetEngine(), func);
		}
	}
}

void Photon::scriptPrint(std::string val) 	{ std::cout<<val<<"\n"; }
void Photon::scriptPrint(int32_t val) 		{ std::cout<<val<<"\n"; }
void Photon::scriptPrint(uint32_t val) 		{ std::cout<<val<<"\n"; }
void Photon::scriptPrint(int64_t val) 		{ std::cout<<val<<"\n"; }
void Photon::scriptPrint(uint64_t val) 		{ std::cout<<val<<"\n"; }
void Photon::scriptPrint(double val) 		{ std::cout<<val<<"\n"; }
void Photon::scriptPrint(math::vec2 val) 	{ std::cout<<val<<"\n"; }
void Photon::scriptPrint(math::vec3 val) 	{ std::cout<<val<<"\n"; }
void Photon::scriptPrint(math::vec4 val) 	{ std::cout<<val<<"\n"; }
void Photon::scriptPrint(math::mat4& val) 	{ std::cout<<val; }

void Photon::scriptMessageCallback(const asSMessageInfo *msg) {
	switch(msg->type) {
		case asMSGTYPE_ERROR: 		spdlog::error("[{}:{}] {} : {}", msg->row, msg->col, msg->section, msg->message); break;
		case asMSGTYPE_WARNING: 	spdlog::warn("[{}:{}] {} : {}", msg->row, msg->col, msg->section, msg->message); break;
		case asMSGTYPE_INFORMATION: spdlog::info("[{}:{}] {} : {}", msg->row, msg->col, msg->section, msg->message); break;
	}
}

std::shared_ptr<photonvk::Mesh> Photon::createMesh(CScriptArray &vertices) {
	std::vector<Vertex> tmp(vertices.GetSize());
	memcpy(tmp.data(), vertices.GetBuffer(), vertices.GetSize() * sizeof(Vertex));
	return createMesh(tmp);
}

std::shared_ptr<photonvk::Mesh> Photon::createMeshIndexed(CScriptArray &vertices, CScriptArray &indices) {
	std::vector<Vertex> tmpVertices(vertices.GetSize());
	memcpy(tmpVertices.data(), vertices.GetBuffer(), vertices.GetSize() * sizeof(Vertex));
	std::vector<uint16_t> tmpIndices(indices.GetSize());
	memcpy(tmpIndices.data(), indices.GetBuffer(), indices.GetSize() * sizeof(uint16_t));
	return createMeshIndexed(tmpVertices, tmpIndices);
}