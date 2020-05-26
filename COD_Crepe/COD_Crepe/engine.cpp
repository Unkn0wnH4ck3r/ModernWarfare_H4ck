#include <iostream>
#include <Windows.h>
#include <intrin.h>
#include <vector>
#include "dllmain.h"
#include "engine.h"
#include "sdk.h"
#include "obfuscation.h"
#include "spoof_call.hpp"
#include "VMProtectSDK.h"
#pragma comment(lib, "VMProtectSDK64.lib")
using namespace std;
extern "C" void* _silent_crash();

uint64_t imagebase = NULL;
DWORD pebindex = NULL;
HANDLE hMainThread = NULL;

R_AddCmdDrawText_t fnR_AddCmdDrawText = NULL;
R_RegisterFont_t fnR_RegisterFont = NULL;
R_RegisterShader_t fnR_RegisterShader = NULL;
R_RegisterTag_t fnR_RegisterTag = NULL;
CL_DrawStretchPicPhysical_t fnCL_DrawStretchPicPhysical = NULL;
CG_DrawRotatedPicPhysicalW_t fnCG_DrawRotatedPicPhysicalW = NULL;
R_TextWidth_t fnR_TextWidth = NULL;
GetTagPos_t fnGetTagPos = NULL;
IsIngame_t fnIsIngame = NULL;

Rect* drawRect;
Color* whiteColor;
Color* redColor;
Color* orangeColor;
Color* blackColor;
Color* greenColor;
Color* nameColor;
Color* distanceColor;
Color* itemColor;
Color* radarbgColor;

Color* menutextColor;
Color* titlebgColor;
Color* titleoutlineColor;
Color* titleColor;

Color* menubgColor;
Color* menuoutlineColor;
Color* menuColor;
Color* menulineColor;

Color* checkboxonColor;
Color* checkboxoffColor;

Color* healthbarColor;
Color* healthgreenColor;
Color* healthorangeColor;
Color* healthredColor;

__int64 regularFont;
__int64 materialWhite;

uint64_t cg_entities = NULL;
uint64_t cg_bones = NULL;
uint64_t cg_clientinfos = NULL;
uint64_t inputptr = NULL;
uint64_t refdef = NULL;
uint64_t brdata_instance = NULL;

int s_width;
int s_height;

LONGLONG ExpiredDate = NULL;

namespace draw
{
	void IW8_DrawText(int x, int y, Color* color, const char *fmt, ...)
	{
		char str[128];
		va_list va_alist;
		va_start(va_alist, fmt);
		_vsnprintf_s(str, sizeof(str), sizeof(str) - strlen(str), fmt, va_alist);
		va_end(va_alist);
		fnR_AddCmdDrawText(str, 0x7FFFFFFF, regularFont, 0, 15, x + 1, y + 1, 1, 1, 1, 0, blackColor->toFloat(), 0, false, 0, false, 0);
		fnR_AddCmdDrawText(str, 0x7FFFFFFF, regularFont, 0, 15, x, y, 1, 1, 1, 0, color->toFloat(), 0, false, 0, false, 0);
	}
	void IW8_DrawTextCenter(int x, int y, Color* color, const char *fmt, ...)
	{
		char str[128];
		va_list va_alist;
		va_start(va_alist, fmt);
		_vsnprintf_s(str, sizeof(str), sizeof(str) - strlen(str), fmt, va_alist);
		va_end(va_alist);
		int strwidth = strlen(str) * 7.5;
		fnR_AddCmdDrawText(str, 0x7FFFFFFF, regularFont, 0, 15, x - strwidth / 2 + 1, y + 1, 1, 1, 1, 0, blackColor->toFloat(), 0, false, 0, false, 0);
		fnR_AddCmdDrawText(str, 0x7FFFFFFF, regularFont, 0, 15, x - strwidth / 2, y, 1, 1, 1, 0, color->toFloat(), 0, false, 0, false, 0);
	}
	void IW8_DrawRectangle(float x, float y, float width, float height, Color* color)
	{
		//Top
		drawRect->getRect(x, y, width, 1);
		fnCL_DrawStretchPicPhysical(drawRect->toFloat(), 0, 0, 1, 0, color->toFloat(), materialWhite);
		//Left
		drawRect->getRect(x, y, 1, height);
		fnCL_DrawStretchPicPhysical(drawRect->toFloat(), 0, 0, 1, 0, color->toFloat(), materialWhite);
		//Right
		drawRect->getRect(x + width - 1, y, 1, height);
		fnCL_DrawStretchPicPhysical(drawRect->toFloat(), 0, 0, 1, 0, color->toFloat(), materialWhite);
		//Bottom
		drawRect->getRect(x, y + height, width, 1);
		fnCL_DrawStretchPicPhysical(drawRect->toFloat(), 0, 0, 1, 0, color->toFloat(), materialWhite);
	}
	void IW8_DrawRectangleFilled(float x, float y, float width, float height, Color* color)
	{
		drawRect->getRect(x, y, width, height);
		fnCL_DrawStretchPicPhysical(drawRect->toFloat(), 0, 0, 1, 0, color->toFloat(), materialWhite);
	}
	void IW8_DrawLine(float x1, float y1, float x2, float y2, Color* color, float width)
	{
		float l1 = x2 - x1, h = y2 - y1, l2 = sqrt(l1 * l1 + h * h);
		fnCG_DrawRotatedPicPhysicalW(x1 + ((l1 - l2) / 2.f), y1 + (h / 2.f), l2, width, 0, 0, 0, 0, (180.f / M_PI * atan(h / l1)), 0, 0, color->toFloat(), materialWhite);
	}

	struct itemText_s
	{
		Vector3 position;
		int item_id;
		bool grouped;
	};
	vector<itemText_s> itemText_vec;
	void IW8_AddItemText(Vector3 position, int item_id)
	{
		if (itemText_vec.size() < 512) {
			itemText_s itemText;
			itemText.position = position;
			itemText.item_id = item_id;
			itemText.grouped = false;
			itemText_vec.push_back(itemText);
		}
	}

	void IW8_DrawTitleText(int x, int y, Color* color, const char *fmt, ...)
	{
		char str[128];
		va_list va_alist;
		va_start(va_alist, fmt);
		_vsnprintf_s(str, sizeof(str), sizeof(str) - strlen(str), fmt, va_alist);
		va_end(va_alist);
		fnR_AddCmdDrawText(str, 0x7FFFFFFF, regularFont, 0, 20, x, y, 1, 1, 1, 0, color->toFloat(), 0, false, 0, false, 0);
	}
	void IW8_DrawMenuText(int x, int y, Color* color, const char *fmt, ...)
	{
		char str[128];
		va_list va_alist;
		va_start(va_alist, fmt);
		_vsnprintf_s(str, sizeof(str), sizeof(str) - strlen(str), fmt, va_alist);
		va_end(va_alist);
		fnR_AddCmdDrawText(str, 0x7FFFFFFF, regularFont, 0, 15, x, y, 1, 1, 1, 0, color->toFloat(), 0, false, 0, false, 0);
	}
}

namespace regular
{
	uint64_t get_entity(int i)
	{
		uint64_t ptr = (uint64_t)(cg_entities + (i * offsets::ENT_MAXSIZE_OFFSET));
		return ptr;
	}
	uint64_t get_localentity()
	{
		return *(uint64_t*)(cg_clientinfos + offsets::CI_LOCAL_OFFSET);
	}
	uint64_t get_entityinfo(int i)
	{
		uint64_t infoBase = *(uint64_t*)(cg_clientinfos + offsets::EI_BASE_OFFSET);
		if (infoBase)
			return (uint64_t)(infoBase + (i * offsets::EI_MAXSIZE_OFFSET));
		return NULL;
	}
	uint64_t get_item(int i)
	{
		uint64_t ptr = (uint64_t)(imagebase + offsets::ITEM_BASE_OFFSET + (i * offsets::ITEM_MAXSIZE_OFFSET));
		return ptr;
	}
	char* get_playername(int i)
	{
		uint64_t nameArray = *(uint64_t*)(imagebase + offsets::NAME_ARRAY_OFFSET);
		if (nameArray) {
			playername_s* player_name = (playername_s*)(nameArray + offsets::NAME_LIST_OFFSET + ((i + (i * 8)) << 0x4));
			return player_name->szName;
		}
		return XorString("player");
	}
	bool get_bone_pos(int i, uint64_t entity, int bone_index, Vector3* bone_pos)
	{
		unsigned short index = *(unsigned short*)(imagebase + offsets::INDEX_ARRAY_OFFSET + (i * sizeof(index)));
		float vecOut[7] = { 0 };
		if (index && fnGetTagPos(entity + offsets::ENT_SKELETON, cg_bones + index * offsets::BONE_MAXSIZE_OFFSET, bone_index, vecOut)) {
			bone_pos->x = vecOut[0];
			bone_pos->y = vecOut[1];
			bone_pos->z = vecOut[2];
			return true;
		}
		return false;
	}
	uint32_t xor_for_viewangle(uint64_t pointer, uint32_t value)
	{
		uint32_t xorvalue1 = *(uint32_t*)(inputptr + offsets::INPUT_SEED_OFFSET);//random_seed_viewangle
		uint32_t xorvalue2 = pointer;
		uint32_t xorvalue = (((xorvalue1 ^ xorvalue2) + 2) * (xorvalue1 ^ xorvalue2)) ^ value;
		return xorvalue;
	}
	Vector3 get_vieworigin()
	{
		return *(Vector3*)(cg_clientinfos + offsets::CI_VIEWORIGIN_OFFSET);
	}
	Vector2 get_viewangle() {
		Vector2 viewangle = Vector2(0, 0);

		viewangle.y = *(float*)(cg_clientinfos + offsets::CI_VIEWANGLE_OFFSET);//标准化正常viewangle
		viewangle.x = 360.f - *(float*)(cg_clientinfos + offsets::CI_VIEWANGLE_OFFSET + 4);
		if (viewangle.y > 85.0f)
			viewangle.y = viewangle.y - 360.0f;
		viewangle.x -= 270.f;

		return viewangle;
	}
	Vector2 calculate_angle(Vector3 src, Vector3 dst) {
		Vector2 angles = Vector2(0, 0);

		Vector3 delta = src - dst;
		float hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);

		angles.x = std::atanf(delta.x / delta.y) * 57.2957795131f;
		angles.y = std::atanf(-delta.z / hyp) * -57.2957795131f;

		if (delta.y >= 0.0f)
			angles.x -= 180.0f;

		return angles;
	}
	void aim_at(Vector3 dst)
	{
		uint32_t viewangle_y_xor = regular::xor_for_viewangle(inputptr + offsets::INPUT_ANGLE_OFFSET, *(uint32_t*)(inputptr + offsets::INPUT_ANGLE_OFFSET));//内存angle可修改区域
		uint32_t viewangle_x_xor = regular::xor_for_viewangle(inputptr + offsets::INPUT_ANGLE_OFFSET + 0x4, *(uint32_t*)(inputptr + +offsets::INPUT_ANGLE_OFFSET + 0x4));
		float* viewangle_y = (float*)&viewangle_y_xor;
		float* viewangle_x = (float*)&viewangle_x_xor;

		Vector2 viewangle_ci = get_viewangle();

		Vector2 delta_angle = calculate_angle(get_vieworigin(), dst);//计算delta

		float viewangle_y_new = *viewangle_y + (delta_angle.y - viewangle_ci.y);
		float viewangle_x_new = *viewangle_x - (delta_angle.x - viewangle_ci.x);
		int32_t viewangle_y_new_xor = *(int32_t*)&viewangle_y_new;
		int32_t viewangle_x_new_xor = *(int32_t*)&viewangle_x_new;

		*(uint32_t*)(inputptr + offsets::INPUT_ANGLE_OFFSET) = regular::xor_for_viewangle(inputptr + offsets::INPUT_ANGLE_OFFSET, viewangle_y_new_xor);//加密angle
		*(uint32_t*)(inputptr + offsets::INPUT_ANGLE_OFFSET + 0x4) = regular::xor_for_viewangle(inputptr + offsets::INPUT_ANGLE_OFFSET + 0x4, viewangle_x_new_xor);
	}
	bool WorldToScreen(Vector3 vOrigin, Vector2* vOut)
	{
		Vector3* viewMatrix = (Vector3*)(refdef + offsets::REF_MATRIX_OFFSET);
		float FovX = *(float*)(refdef + offsets::REF_FOVX_OFFSET);
		float FovY = *(float*)(refdef + offsets::REF_FOVY_OFFSET);

		Vector3 vLocal, vTrans = Vector3(0, 0, 0);

		vLocal = vOrigin - get_vieworigin();

		vTrans.x = vLocal.Dot(viewMatrix[1]);
		vTrans.y = vLocal.Dot(viewMatrix[2]);
		vTrans.z = vLocal.Dot(viewMatrix[0]);

		if (vTrans.z < 0.01f)
			return false;

		vOut->x = ((s_width / 2) * (1 - (vTrans.x / FovX / vTrans.z)));
		vOut->y = ((s_height / 2) * (1 - (vTrans.y / FovY / vTrans.z)));

		return true;
	}

	void connect_bones(Vector2* bones_screenPos, long count, Color* color)
	{
		long last_count = count - 1;
		for (long i = 0; i < last_count; ++i)
			draw::IW8_DrawLine(bones_screenPos[i].x, bones_screenPos[i].y, bones_screenPos[i + 1].x, bones_screenPos[i + 1].y, color, 1.0f);
	}
	bool bones_to_screen(Vector3* bonepos_array, Vector2* screenpos_array, int array_count)
	{
		for (int i = 0; i < array_count; ++i) {
			if (!WorldToScreen(bonepos_array[i], &screenpos_array[i]))
				return false;
		}
		return true;
	}
	void draw_skeleton(int i, uint64_t entity, Color* color)
	{
		Vector3 header_to_bladder[6], right_foot_to_bladder[5], left_foot_to_bladder[5], right_hand[5], left_hand[5];
		Vector2 screen_header_to_bladder[6], screen_right_foot_to_bladder[5], screen_left_foot_to_bladder[5], screen_right_hand[5], screen_left_hand[5];

		if (get_bone_pos(i, entity, BONE_INDEX::BONE_POS_HEAD, &header_to_bladder[0]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_NECK, &header_to_bladder[1]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_CHEST, &header_to_bladder[2]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_MID, &header_to_bladder[3]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_TUMMY, &header_to_bladder[4]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_PELVIS, &header_to_bladder[5]) &&

			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_RIGHT_FOOT_1, &right_foot_to_bladder[1]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_RIGHT_FOOT_2, &right_foot_to_bladder[2]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_RIGHT_FOOT_3, &right_foot_to_bladder[3]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_RIGHT_FOOT_4, &right_foot_to_bladder[4]) &&

			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_LEFT_FOOT_1, &left_foot_to_bladder[1]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_LEFT_FOOT_2, &left_foot_to_bladder[2]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_LEFT_FOOT_3, &left_foot_to_bladder[3]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_LEFT_FOOT_4, &left_foot_to_bladder[4]) &&

			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_LEFT_HAND_1, &right_hand[1]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_LEFT_HAND_2, &right_hand[2]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_LEFT_HAND_3, &right_hand[3]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_LEFT_HAND_4, &right_hand[4]) &&

			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_RIGHT_HAND_1, &left_hand[1]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_RIGHT_HAND_2, &left_hand[2]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_RIGHT_HAND_3, &left_hand[3]) &&
			get_bone_pos(i, entity, BONE_INDEX::BONE_POS_RIGHT_HAND_4, &left_hand[4]))
		{
			right_foot_to_bladder[0] = header_to_bladder[5];
			left_foot_to_bladder[0] = header_to_bladder[5];
			right_hand[0] = header_to_bladder[2];
			left_hand[0] = header_to_bladder[2];

			if (!bones_to_screen(header_to_bladder, screen_header_to_bladder, 6))
				return;

			if (!bones_to_screen(right_foot_to_bladder, screen_right_foot_to_bladder, 5))
				return;

			if (!bones_to_screen(left_foot_to_bladder, screen_left_foot_to_bladder, 5))
				return;

			if (!bones_to_screen(right_hand, screen_right_hand, 5))
				return;

			if (!bones_to_screen(left_hand, screen_left_hand, 5))
				return;

			connect_bones(screen_header_to_bladder, 6, color);
			connect_bones(screen_right_foot_to_bladder, 5, color);
			connect_bones(screen_left_foot_to_bladder, 5, color);
			connect_bones(screen_right_hand, 5, color);
			connect_bones(screen_left_hand, 5, color);
		}
	}

	Vector2 get_entityinfo_angle(uint64_t entityinfo)
	{
		Vector2 results = Vector2(0, 0);
		Vector2* angle = (Vector2*)(entityinfo + offsets::EI_ANGLE_OFFSET);
		results.x = angle->y < 0.0f ? (360.0f + angle->y) : angle->y;
		results.y = angle->x <= 0.0f ? fabs(angle->x) : 360.0f - angle->x;
		return results;
	}

	bool is_key_down(int key)
	{
		return *(bool*)(imagebase + offsets::KEYSTATE_OFFSET + key * 4 + 3);//lea rsi,[rsi+r12*4];mov [rsi+03],dil
		//0x88 Alt
		//0x8A Ctrl
		//0x94 Ins
		//0x95 Del
		//0x96 PageDown
		//0x97 PageUp
		//0x98 HOME
		//0x9A F1
	}
	struct keyinfo_s {
		int keyid;
		bool bpressed;
	};
	vector <keyinfo_s> keys;
	bool is_key_pressed(int keyId) {
		int i;
		for (i = 0; i < keys.size(); i++) {
			if (keys[i].keyid == keyId) {
				bool bpressed = is_key_down(keyId);
				if (bpressed != keys[i].bpressed) {
					keys[i].bpressed = bpressed;
					if (bpressed)
						return true;
					else
						return false;
				}
				return false;
			}
		}
		if (i == keys.size()) {
			keyinfo_s key;
			key.keyid = keyId;
			key.bpressed = is_key_down(key.keyid);
			keys.push_back(key);
			return key.bpressed;
		}
		return false;
	}

	uintptr_t get_brdata(uint64_t instance, int i)
	{
		return instance + offsets::BRDATA_BASE_OFFSET + ((i + i * 8) << 4);
	}
}

namespace control
{
	bool debugmode = false;

	bool drawPlayer = true;
	bool drawPlayerbox = false;
	bool drawPlayername = true;
	bool drawPlayerdist = true;
	bool drawPlayerskeleton = true;
	bool drawPlayerline = false;
	bool drawHealth = true;

	bool drawItem = true;
	bool drawItemweapon = true;
	bool drawItemammo = true;
	bool drawItemcash = true;
	bool drawItemother = true;

	bool drawVehicle = true;
	bool drawRadar = true;

	bool aimbot = true;
	bool norecoil = true;

	int maxPlayerDistance = 500;
	int maxVehicleDistance = 500;
	int maxItemDistance = 100;
	int maxRadarDistance = 1000;
	int maxAimbotDistance = 200;

	bool drawMenu = true;
}

struct speedinfo_s {
	int id;
	Vector3 last_velocity;
	Vector3 last_position;
	DWORD last_tickcount;
};
speedinfo_s speedtable[150] = { 0 };

bool bLockTarget = false;
int bestEntityId = 0;
void DrawEsp()
{
	VMProtectBeginMutation("Crepe::DrawEsp");
	bool bAimbotKey = false;
	bool bFoundTarget = false;
	float tmpAimfov = 360;

	cg_entities = decrypt::decrypt_for_entity(imagebase, pebindex);
	cg_bones = decrypt::decrypt_for_bone(imagebase, pebindex);
	cg_clientinfos = decrypt::decrypt_for_clientinfo(imagebase);
	inputptr = decrypt::decrypt_for_inputptr(imagebase, pebindex);
	refdef = decrypt::decrypt_for_refdef(imagebase);
	brdata_instance = *(uint64_t*)(imagebase + offsets::BRDATA_INSTANCE_OFFSET);

	if (control::debugmode) {
		draw::IW8_DrawText(200, 160, whiteColor, XorString("Unkn0wnH4ck3r Debug Console\n"));
		draw::IW8_DrawText(200, 180, whiteColor, XorString("regularFont = %p\n"), regularFont);
		draw::IW8_DrawText(200, 200, whiteColor, XorString("materialWhite = %p\n"), materialWhite);
		draw::IW8_DrawText(200, 220, whiteColor, XorString("cg_entities = %p\n"), cg_entities);
		draw::IW8_DrawText(200, 240, whiteColor, XorString("cg_bones = %p\n"), cg_bones);
		draw::IW8_DrawText(200, 260, whiteColor, XorString("cg_clientinfos = %p\n"), cg_clientinfos);
		draw::IW8_DrawText(200, 280, whiteColor, XorString("pebindex = %d\n"), pebindex);
		draw::IW8_DrawText(200, 300, whiteColor, XorString("inputptr = %p\n"), inputptr);//4 14
		draw::IW8_DrawText(200, 320, whiteColor, XorString("refdef = %p\n"), refdef);
		draw::IW8_DrawText(200, 340, whiteColor, XorString("brdata_instance = %p\n"), brdata_instance);
	}

	if (!cg_entities || !cg_bones || !cg_clientinfos || !refdef || !inputptr || !brdata_instance || !fnIsIngame())
		return;

	s_width = *(int*)(refdef + offsets::REF_WIDTH_OFFSET);
	s_height = *(int*)(refdef + offsets::REF_HEIGHT_OFFSET);

	uint64_t local_entity = regular::get_localentity();
	short local_index = 0;
	if (!local_entity || !(*(BYTE*)(local_entity + offsets::ENT_VAILD_OFFSET) & 1))
		return;
	local_index = *(short*)(local_entity + offsets::ENT_INDEX_OFFSET);//获取自身ID

	int local_teamid = 0;
	uint64_t local_entity_info = regular::get_entityinfo(local_index);
	if (!local_entity_info)
		return;
	local_teamid = *(int*)(local_entity_info + offsets::EI_TEAMID_OFFSET);//获取自身队伍ID

	Vector3 local_position = decrypt::decrypt_for_entitypos(imagebase, local_entity);
	if (local_position.x == 0 || local_position.y == 0 || local_position.z == 0)//过滤坐标
		return;

	if (control::debugmode) {
		draw::IW8_DrawText(200, 360, whiteColor, XorString("local_entity = %p\n"), local_entity);
		draw::IW8_DrawText(200, 380, whiteColor, XorString("local_index = %d\n"), local_index);
		draw::IW8_DrawText(200, 400, whiteColor, XorString("local_entity_info = %p\n"), local_entity_info);
		draw::IW8_DrawText(200, 420, whiteColor, XorString("local_teamid = %d\n"), local_teamid);
		draw::IW8_DrawText(200, 440, whiteColor, XorString("local_position = %.2f,%.2f,%.2f\n"), local_position.x, local_position.y, local_position.z);
		draw::IW8_DrawText(200, 460, whiteColor, XorString("local_brdata = %p\n"), regular::get_brdata(brdata_instance, local_index));
	}
	if (control::drawRadar) {
		draw::IW8_DrawRectangleFilled(s_width - 250, 100, 200, 200, radarbgColor);
		draw::IW8_DrawLine(s_width - 250, 200, s_width - 50, 200, greenColor, 1.f);
		draw::IW8_DrawLine(s_width - 150, 100, s_width - 150, 300, greenColor, 1.f);
		draw::IW8_DrawRectangle(s_width - 250, 100, 200, 200, whiteColor);
		draw::IW8_DrawRectangleFilled(s_width - 150 - 2, 200 - 2, 4, 4, greenColor);
	}
	if (control::aimbot && regular::is_key_down(0xBC))//鼠标左键
		bAimbotKey = true;
	else
		bLockTarget = false;
	for (int i = 0; i < draw::itemText_vec.size(); i++) {//物品聚类
		if (!draw::itemText_vec[i].grouped) {
			Vector3 i_vecPosition = draw::itemText_vec[i].position;
			Vector2 i_vecScreenPos = Vector2(0, 0);
			if (!regular::WorldToScreen(i_vecPosition, &i_vecScreenPos))
				continue;
			int dwCount = 0;
			for (int j = 0; j < draw::itemText_vec.size(); j++) {
				if (dwCount > 9) {
					draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString("..."));
					break;
				}
				if (!draw::itemText_vec[j].grouped) {
					Vector3 j_vecPosition = draw::itemText_vec[j].position;
					Vector2 j_vecScreenPos = Vector2(0, 0);
					if (!regular::WorldToScreen(j_vecPosition, &j_vecScreenPos))
						continue;
					float dwTextDis = sqrt(pow(i_vecScreenPos.x - j_vecScreenPos.x, 2) + pow(i_vecScreenPos.y - j_vecScreenPos.y, 2));
					float dwWorldDis = sqrt(pow(i_vecPosition.x - j_vecPosition.x, 2) + pow(i_vecPosition.y - j_vecPosition.y, 2) + pow(i_vecPosition.z - j_vecPosition.z, 2)) / 40;
					if (dwTextDis < 100 && dwWorldDis < 5) {
						draw::itemText_vec[j].grouped = true;
						float fdistance = local_position.Distance(i_vecPosition) / 40;

						if (dwCount == 0)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y - 15, itemColor, XorString("[%dM]"), (int)fdistance);
	
						if (draw::itemText_vec[j].item_id == 1)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString(u8"| 装甲板"));
						else if (draw::itemText_vec[j].item_id == 2)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString(u8"| 突击步枪/轻机枪 弹药"));
						else if (draw::itemText_vec[j].item_id == 3)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString(u8"| 射手/狙击步枪 弹药"));
						else if (draw::itemText_vec[j].item_id == 4)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString(u8"| 突击步枪"));
						else if (draw::itemText_vec[j].item_id == 5)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString(u8"| 射手步枪"));
						else if (draw::itemText_vec[j].item_id == 6)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString(u8"| 冲锋枪"));
						else if (draw::itemText_vec[j].item_id == 7)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString(u8"| 轻机枪"));
						else if (draw::itemText_vec[j].item_id == 8)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString(u8"| 现金"));
						else if (draw::itemText_vec[j].item_id == 9)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString(u8"| 物资箱"));
						else if (draw::itemText_vec[j].item_id == 10)
							draw::IW8_DrawText(i_vecScreenPos.x - 10, i_vecScreenPos.y + dwCount * 15, itemColor, XorString(u8"| 补给站"));
						dwCount++;
					}
				}
			}
		}
	}
	draw::itemText_vec.clear();
	for (int i = 0; i < 600; ++i) {
		uint64_t entity = regular::get_entity(i);
		if (!entity)
			continue;

		Vector3 position = decrypt::decrypt_for_entitypos(imagebase, entity);
		if (position.x == 0 || position.y == 0 || position.z == 0)//过滤坐标
			continue;
		float fdistance = local_position.Distance(position) / 40;

		short entity_type = *(short*)(entity + offsets::ENT_TYPE_OFFSET);
		if (entity_type == 1) {
			short entity_index = *(short*)(entity + offsets::ENT_INDEX_OFFSET);
			if (entity_index != i || entity_index == local_index)//过滤自己
				continue;

			uint64_t entityinfo = regular::get_entityinfo(i);
			if (!entityinfo)
				continue;
			int entityinfo_teamid = *(int*)(entityinfo + offsets::EI_TEAMID_OFFSET);
			if(entityinfo_teamid == local_teamid)//过滤队友
				continue;

			if (*(BYTE*)(entity + offsets::ENT_VAILD_OFFSET) & 1) {//过滤无效实体
				uint8_t entity_stance = *(uint8_t*)(entity + offsets::ENT_STANCE_OFFSET);
				Vector3 root_position = position;

				if (entity_stance == 0)//站立
					root_position.z += 68.f;
				else if (entity_stance == 4)//下蹲
					root_position.z += 48.f;
				else if (entity_stance == 8 || entity_stance == 16)//伏地 倒地
					root_position.z += 30.f;
				else
					continue;

				//if (entity_index >= 0 && entity_index < 150) {
				//	DWORD current_tick = GetTickCount();
				//	if (current_tick - speedtable[entity_index].last_tickcount > 100) {
				//		speedtable[entity_index].last_velocity = (position - speedtable[entity_index].last_position) / (current_tick - speedtable[entity_index].last_tickcount) * 1000.f;
				//		speedtable[entity_index].last_position = position;
				//		speedtable[entity_index].last_tickcount = current_tick;
				//	}
				//}

				if (fdistance > control::maxRadarDistance)
					continue;
				if (control::drawRadar) {
					Vector2 local_angle = regular::get_entityinfo_angle(local_entity_info);
					float sin_yaw = sin(local_angle.x * (M_PI / 180));
					float minus_cos_yaw = -cos(local_angle.x * (M_PI / 180));
					float dx = (position.x - local_position.x);
					float dy = (position.y - local_position.y);
					float x = dy * minus_cos_yaw + dx * sin_yaw;
					float y = dx * minus_cos_yaw - dy * sin_yaw;
					float range = 10000;
					if (fabs(x) > range || fabs(y) > range)
					{
						if (y > x)
						{
							if (y > -x) {
								x = range * x / y;
								y = range;
							}
							else {
								y = -range * y / x;
								x = -range;
							}
						}
						else {
							if (y > -x) {
								y = range * y / x;
								x = range;
							}
							else {
								x = -range * x / y;
								y = -range;
							}
						}
					}
					Vector2 radarscreenpos;
					radarscreenpos.x = s_width - 150 + int(x / range * 100.0f);
					radarscreenpos.y = 200 + int(y / range * 100.0f);
					draw::IW8_DrawRectangleFilled(radarscreenpos.x - 2, radarscreenpos.y - 2, 4, 4, redColor);
				}

				if (fdistance < control::maxAimbotDistance && !bLockTarget) {
					Vector3 middle_position = position;
					middle_position.z += (root_position.z - position.z) / 2.0f;

					Vector2 delta = regular::calculate_angle(regular::get_vieworigin(), middle_position) - regular::get_viewangle();
					if (delta.Length() <= tmpAimfov) {
						tmpAimfov = delta.Length();
						bestEntityId = entity_index;
						bFoundTarget = true;
					}
				}

				if (fdistance > control::maxPlayerDistance)
					continue;

				Vector2 positionTransformed = Vector2(0, 0);
				if (!regular::WorldToScreen(position, &positionTransformed))
					continue;
				Vector2 root_positionTransformed = Vector2(0, 0);
				if (!regular::WorldToScreen(root_position, &root_positionTransformed))
					continue;

				if (control::drawPlayer && control::drawPlayerline)
					draw::IW8_DrawLine(positionTransformed.x, positionTransformed.y, s_width / 2, s_height, redColor, 1.4f);

				if (control::drawPlayer && control::drawPlayerskeleton)
					regular::draw_skeleton(i, entity, orangeColor);

				float bb_width = sqrt(pow(positionTransformed.x - root_positionTransformed.x, 2) + pow(positionTransformed.y - root_positionTransformed.y, 2)) / 1.5f;
				float bb_height = sqrt(pow(positionTransformed.x - root_positionTransformed.x, 2) + pow(positionTransformed.y - root_positionTransformed.y, 2));
				if (control::drawPlayer && control::drawPlayerbox)
					draw::IW8_DrawRectangle(root_positionTransformed.x - bb_width / 2, root_positionTransformed.y, bb_width, bb_height, whiteColor);
				if (control::drawPlayer && control::drawPlayername)
					draw::IW8_DrawTextCenter(positionTransformed.x, positionTransformed.y - bb_height / 2 + 15, nameColor, regular::get_playername(entity_index));
				if (control::drawPlayer && control::drawPlayerdist)
					draw::IW8_DrawTextCenter(positionTransformed.x, positionTransformed.y - bb_height / 2 + 30, distanceColor, XorString("[%dM]"), (int)fdistance);

				if (control::drawPlayer && control::drawHealth) {
					uintptr_t brdata = regular::get_brdata(brdata_instance, entity_index);
					if (!brdata)
						continue;

					int brdata_health = *(int*)(brdata + offsets::BRDATA_HEALTH);
					draw::IW8_DrawRectangleFilled(root_positionTransformed.x - 30, root_positionTransformed.y - 15, 60, 8, healthbarColor);
					if (brdata_health >= 80)
						draw::IW8_DrawRectangleFilled(root_positionTransformed.x - 28, root_positionTransformed.y - 13, (int)brdata_health * 0.44, 4, healthgreenColor);
					else if (brdata_health >= 60)
						draw::IW8_DrawRectangleFilled(root_positionTransformed.x - 28, root_positionTransformed.y - 13, (int)brdata_health * 0.44, 4, healthorangeColor);
					else
						draw::IW8_DrawRectangleFilled(root_positionTransformed.x - 28, root_positionTransformed.y - 13, (int)brdata_health * 0.44, 4, healthredColor);
				}
			}
		}
		else if (entity_type == 14 && control::drawVehicle) {
			if (fdistance > control::maxVehicleDistance)
				continue;

			Vector2 positionTransformed;
			if (regular::WorldToScreen(position, &positionTransformed))
				draw::IW8_DrawTextCenter(positionTransformed.x, positionTransformed.y, whiteColor, XorString(u8"车辆 [%dM]"), (int)fdistance);
		}
	}
	for (int i = 0; i < 12; i++) {
		uint32_t item_mask = *(uint32_t*)(imagebase + offsets::ITEM_MASK_OFFSET + i * 4);//一个Mask区域只能存32个物品的有效性
		if (!item_mask)
			break;
		while (item_mask) {
			uint32_t index = 0;
			bool bfound = !_BitScanReverse((unsigned long *)&index, item_mask);
			int value = index ^ 0x1F;
			if (bfound)
				value = 0x20;
			item_mask &= ~(0x80000000 >> value);

			int item_index = value + 32 * i;
			uint64_t item = regular::get_item(item_index);
			Vector3 position = decrypt::decrypt_for_itempos(imagebase, item);
			if (position.x == 0 || position.y == 0 || position.z == 0)
				continue;
			float fdistance = local_position.Distance(position) / 40;

			if (fdistance > control::maxItemDistance)
				continue;

			uint64_t item_type = *(uint64_t*)item;
			char* item_name = *(char**)item_type;

			if (control::drawItem) {
				if (strstr(item_name, XorString("wm_armor")) && control::drawItemother)
					draw::IW8_AddItemText(position, 1);
				else if (strstr(item_name, XorString("ammo_box_ar")) && control::drawItemammo)
					draw::IW8_AddItemText(position, 2);
				else if (strstr(item_name, XorString("ammo_marksman_sniper")) && control::drawItemammo)
					draw::IW8_AddItemText(position, 3);
				else if (strstr(item_name, XorString("weapon_wm_ar")) && control::drawItemweapon)
					draw::IW8_AddItemText(position, 4);
				else if (strstr(item_name, XorString("weapon_wm_sn")) && control::drawItemweapon)
					draw::IW8_AddItemText(position, 5);
				else if (strstr(item_name, XorString("weapon_wm_sm")) && control::drawItemweapon)
					draw::IW8_AddItemText(position, 6);
				else if (strstr(item_name, XorString("weapon_wm_lm")) && control::drawItemweapon)
					draw::IW8_AddItemText(position, 7);
				else if (strstr(item_name, XorString("accessory_money")) && control::drawItemcash)
					draw::IW8_AddItemText(position, 8);
				else if (strstr(item_name, XorString("military_loot_crate")) && control::drawItemother)
					draw::IW8_AddItemText(position, 9);
				else if (strstr(item_name, XorString("military_purchase_kiosk")) && control::drawItemother)
					draw::IW8_AddItemText(position, 10);
			}
			//Vector2 positionTransformed;
			//if (regular::WorldToScreen(position, &positionTransformed))
			//	draw::IW8_DrawTextCenter(positionTransformed.x, positionTransformed.y, itemColor, XorString("%s [%dM]"), item_name, (int)fdistance);
		}
	}
	if (bAimbotKey && bFoundTarget)
		bLockTarget = true;
	if (bLockTarget) {
		uint64_t entity = regular::get_entity(bestEntityId);
		if (entity && *(BYTE*)(entity + offsets::ENT_VAILD_OFFSET) & 1) {
			Vector3 head_position = Vector3(0, 0, 0);
			if (!regular::get_bone_pos(bestEntityId, entity, BONE_INDEX::BONE_POS_HEAD, &head_position)) {
				head_position = decrypt::decrypt_for_entitypos(imagebase, entity);
				if (head_position.x == 0 || head_position.y == 0 || head_position.z == 0) {
					bLockTarget = false;
					return;
				}
			}

			//float fdistance = local_position.Distance(head_position) / 40.f;
			//float travelTime = fdistance / 750.f;//AC8
			//head_position.x += (speedtable[bestEntityId].last_velocity.x * travelTime);
			//head_position.y += (speedtable[bestEntityId].last_velocity.y * travelTime);
			//head_position.z += (speedtable[bestEntityId].last_velocity.z * travelTime);
			//head_position.z += (9.81f / 40.f * travelTime * travelTime);

			if (control::aimbot)
				regular::aim_at(head_position);
		}
		else {
			bLockTarget = false;
			return;
		}
	}
	if (control::norecoil) {
		*(float*)(cg_clientinfos + offsets::CI_RECOIL_OFFSET) = 0.f;
		*(float*)(cg_clientinfos + offsets::CI_RECOIL_OFFSET + 0x4) = 0.f;
	}
	VMProtectEnd();
}

void DrawMenu()
{
	int menu_x = *(int*)(imagebase + offsets::SCREENSIZE_OFFSET) * 0.02;
	int menu_y = *(int*)(imagebase + offsets::SCREENSIZE_OFFSET + 4) * 0.22;

	draw::IW8_DrawRectangleFilled(menu_x, menu_y, 150, 30, titlebgColor);
	draw::IW8_DrawRectangle(menu_x, menu_y, 150, 30, titleoutlineColor);
	draw::IW8_DrawTitleText(menu_x + 8, menu_y + 25, titleColor, XorString("Call of Duty: MW"));

	draw::IW8_DrawRectangleFilled(menu_x, menu_y + 40, 150, 512, menubgColor);
	draw::IW8_DrawRectangle(menu_x, menu_y + 40, 150, 512, menuoutlineColor);

	draw::IW8_DrawMenuText(menu_x + 55, menu_y + 65, menuColor, XorString(u8"透视类"));
	draw::IW8_DrawRectangleFilled(menu_x + 15, menu_y + 70, 120, 2, menulineColor);

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83, 10, 10, control::drawPlayer ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96, whiteColor, XorString(u8"F1 人物透视"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 1, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 1, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 1, 10, 10, control::drawPlayerbox ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 1, whiteColor, XorString(u8"Ctrl+F1  方框"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 2, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 2, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 2, 10, 10, control::drawPlayername ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 2, whiteColor, XorString(u8"Ctrl+F2  名字"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 3, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 3, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 3, 10, 10, control::drawPlayerdist ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 3, whiteColor, XorString(u8"Ctrl+F3  距离"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 4, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 4, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 4, 10, 10, control::drawPlayerskeleton ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 4, whiteColor, XorString(u8"Ctrl+F4  骨骼"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 5, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 5, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 5, 10, 10, control::drawPlayerline ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 5, whiteColor, XorString(u8"Ctrl+F5  连线"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 6, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 6, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 6, 10, 10, control::drawHealth ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 6, whiteColor, XorString(u8"Ctrl+F6  血量"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 7, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 7, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 7, 10, 10, control::drawItem ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 7, whiteColor, XorString(u8"F2 物品透视"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 8, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 8, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 8, 10, 10, control::drawItemweapon ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 8, whiteColor, XorString(u8"Ctrl+F7  武器"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 9, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 9, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 9, 10, 10, control::drawItemammo ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 9, whiteColor, XorString(u8"Ctrl+F8  弹药"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 10, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 10, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 10, 10, 10, control::drawItemcash ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 10, whiteColor, XorString(u8"Ctrl+F9  现金"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 11, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 11, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 11, 10, 10, control::drawItemother ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 11, whiteColor, XorString(u8"Ctrl+F10其他"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 12, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 12, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 12, 10, 10, control::drawVehicle ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 12, whiteColor, XorString(u8"F3 载具透视"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 80 + 22 * 13, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 80 + 22 * 13, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 83 + 22 * 13, 10, 10, control::drawRadar ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 96 + 22 * 13, whiteColor, XorString(u8"F4 人物雷达"));

	draw::IW8_DrawMenuText(menu_x + 55, menu_y + 407, menuColor, XorString(u8"瞄准类"));
	draw::IW8_DrawRectangleFilled(menu_x + 15, menu_y + 412, 120, 2, menulineColor);

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 422, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 422, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 425, 10, 10, control::aimbot ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 438, whiteColor, XorString(u8"F5 自动瞄准"));

	draw::IW8_DrawRectangleFilled(menu_x + 20, menu_y + 422 + 22 * 1, 16, 15, menubgColor);
	draw::IW8_DrawRectangle(menu_x + 20, menu_y + 422 + 22 * 1, 16, 15, menuoutlineColor);
	draw::IW8_DrawRectangleFilled(menu_x + 23, menu_y + 425 + 22 * 1, 10, 10, control::norecoil ? checkboxonColor : checkboxoffColor);
	draw::IW8_DrawMenuText(menu_x + 50, menu_y + 438 + 22 * 1, whiteColor, XorString(u8"F6 无后坐力"));

	draw::IW8_DrawMenuText(menu_x + 10, menu_y + 472 + 22 * 1, menuColor, XorString(u8"PgUP/Dn 透视 [%dM]"), control::maxPlayerDistance);
	draw::IW8_DrawMenuText(menu_x + 15, menu_y + 472 + 22 * 2, menuColor, XorString(u8"Ins/End 瞄准 [%dM]"), control::maxAimbotDistance);
	draw::IW8_DrawMenuText(menu_x + 25, menu_y + 472 + 22 * 3, menuColor, XorString(u8"Home 隐藏/呼出"));
}

void HotKey()
{
	if (regular::is_key_pressed(0x98)) {
		control::drawMenu = !control::drawMenu;
	}
	if (regular::is_key_down(0x8A)) {
		if (regular::is_key_pressed(0x9A)) {
			control::drawPlayerbox = !control::drawPlayerbox;
		}
		else if (regular::is_key_pressed(0x9B)) {
			control::drawPlayername = !control::drawPlayername;
		}
		else if (regular::is_key_pressed(0x9C)) {
			control::drawPlayerdist = !control::drawPlayerdist;
		}
		else if (regular::is_key_pressed(0x9D)) {
			control::drawPlayerskeleton = !control::drawPlayerskeleton;
		}
		else if (regular::is_key_pressed(0x9E)) {
			control::drawPlayerline = !control::drawPlayerline;
		}
		else if (regular::is_key_pressed(0x9F)) {
			control::drawHealth = !control::drawHealth;
		}

		else if (regular::is_key_pressed(0xA0)) {
			control::drawItemweapon = !control::drawItemweapon;
		}
		else if (regular::is_key_pressed(0xA1)) {
			control::drawItemammo = !control::drawItemammo;
		}
		else if (regular::is_key_pressed(0xA2)) {
			control::drawItemcash = !control::drawItemcash;
		}
		else if (regular::is_key_pressed(0xA3)) {
			control::drawItemother = !control::drawItemother;
		}
	}
	else
	{
		if (regular::is_key_pressed(0x9A)) {
			control::drawPlayer = !control::drawPlayer;
		}
		else if (regular::is_key_pressed(0x9B)) {
			control::drawItem = !control::drawItem;
		}
		else if (regular::is_key_pressed(0x9C)) {
			control::drawVehicle = !control::drawVehicle;
		}
		else if (regular::is_key_pressed(0x9D)) {
			control::drawRadar = !control::drawRadar;
		}
		else if (regular::is_key_pressed(0x9E)) {
			control::aimbot = !control::aimbot;
		}
		else if (regular::is_key_pressed(0x9F)) {
			control::norecoil = !control::norecoil;
		}
	}
	if (regular::is_key_pressed(0x97)) {
		if (control::maxPlayerDistance < 900)
			control::maxPlayerDistance += 50;
	}
	else if (regular::is_key_pressed(0x96)) {
		if (control::maxPlayerDistance > 50)
			control::maxPlayerDistance -= 50;
	}
	if (regular::is_key_pressed(0x94)) {
		if (control::maxAimbotDistance < 900)
			control::maxAimbotDistance += 50;
	}
	else if (regular::is_key_pressed(0x95)) {
		if (control::maxAimbotDistance > 50)
			control::maxAimbotDistance -= 50;
	}
}

VOID WINAPI hLeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	void *RetAddress = _ReturnAddress();
	if (GetCurrentThread() == hMainThread && (reinterpret_cast<uintptr_t>(RetAddress) == imagebase + hook::LOBBY_RET_OFFSET || reinterpret_cast<uintptr_t>(RetAddress) == imagebase + hook::GAME_RET_OFFSET)) {
		if (!control::debugmode) {
			if (!ExpiredDate) {
				if (*(LONGLONG*)((uintptr_t)m_hModule + 0x108) == 90909090) {
					ExpiredDate = *(LONGLONG*)((uintptr_t)m_hModule + 0x100);
				}
				return LeaveCriticalSection(lpCriticalSection);
			}
			else {
				LONGLONG DateDifference = ExpiredDate - GetTickCount();
				if (DateDifference <= 0)
					_silent_crash();
			}
		}
		DrawEsp();
		if (control::drawMenu)
			DrawMenu();
		HotKey();
	}
	//if (GetCurrentThread() == hMainThread)
	//{
	//	printf(XorString("[+] cg_entities = %p\n"), RetAddress);
	//}
	LeaveCriticalSection(lpCriticalSection);
}

void WriteLog(const char* filePath, const char* text)
{
	FILE* fp;
	fopen_s(&fp, filePath, "a");
	if (fp != NULL) {
		SYSTEMTIME st;
		GetLocalTime(&st);
		char buf[128] = { 0 };
		sprintf_s(buf, 128, XorString("%04d%02d%02d %02d:%02d:%02d:%03d "), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		fwrite(buf, sizeof(char), strlen(buf), fp);
		fwrite(text, sizeof(char), strlen(text), fp);
		fwrite("\r\n", sizeof(char), 2, fp);
		fclose(fp);
	}
}

typedef PVOID(WINAPI* RtlAddVectoredHandler_t)(ULONG, PVECTORED_EXCEPTION_HANDLER, int);
static LONG NTAPI ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0xC0000005) {
		char info[256] = { 0 };
		sprintf(info, XorString("ExceptionAddress:%p ExceptionCode:0x%X Rip:%p ModuleBase:%p Offset:0x%X"), ExceptionInfo->ExceptionRecord->ExceptionAddress, ExceptionInfo->ExceptionRecord->ExceptionCode,
			ExceptionInfo->ContextRecord->Rip, m_hModule, (uintptr_t)ExceptionInfo->ExceptionRecord->ExceptionAddress - (uintptr_t)m_hModule);
		WriteLog(XorString("Error.log"), info);
		return EXCEPTION_CONTINUE_SEARCH;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
void SetVEH()
{
	HMODULE kernel32 = GetModuleHandleA(XorString("kernel32.dll"));
	uintptr_t Func = (uintptr_t)GetProcAddress(kernel32, XorString("AddVectoredContinueHandler")) + 0x6;
	Func = Func + *(int32_t*)(Func + 0x1) + 0x5;
	RtlAddVectoredHandler_t RtlAddVectoredExceptionHandler = (RtlAddVectoredHandler_t)Func;
	if (RtlAddVectoredExceptionHandler) {
		RtlAddVectoredExceptionHandler(1, ExceptionHandler, 0);
	}
}

void InitializeEngine()
{
	VMProtectBeginMutation("Crepe::InitializeEngine");
	//AllocConsole();
	//freopen("CON", "w", stdout);
	//printf(XorString("[+] Unkn0wnH4ck3r\n"));
	//imagebase = (uint64_t)GetModuleHandle(NULL);
	//pebindex = (DWORD)(__readgsqword(0x60) >> 12) & 0xF;
	//cg_entities = decrypt::decrypt_for_entity(imagebase, pebindex);
	//cg_bones = decrypt::decrypt_for_bone(imagebase, pebindex);
	//cg_clientinfos = decrypt::decrypt_for_clientinfo(imagebase);
	//inputptr = decrypt::decrypt_for_inputptr(imagebase, pebindex);
	//printf(XorString("[+] cg_entities = %p\n"), cg_entities);
	//printf(XorString("[+] cg_bones = %p\n"), cg_bones);
	//printf(XorString("[+] cg_clientinfos = %p\n"), cg_clientinfos);
	//printf(XorString("[+] inputptr = %p\n"), inputptr);
	//printf(XorString("pebindex = %d\n"), pebindex);
	//return;

	hMainThread = GetCurrentThread();
	imagebase = (uint64_t)GetModuleHandle(NULL);
	pebindex = (DWORD)(__readgsqword(0x60) >> 12) & 0xF;

	fnR_AddCmdDrawText = (R_AddCmdDrawText_t)(imagebase + function::R_ADDCMDDRAWTEXT_OFFSET);
	fnR_RegisterFont = (R_RegisterFont_t)(imagebase + function::R_REGISTERFONT_OFFSET);
	fnR_RegisterShader = (R_RegisterShader_t)(imagebase + function::R_REGISTERSHADER_OFFSET);
	//fnR_RegisterTag = (R_RegisterTag_t)(imagebase + function::R_REGISTERTAG_OFFSET);
	fnCL_DrawStretchPicPhysical = (CL_DrawStretchPicPhysical_t)(imagebase + function::CL_DRAWSTRETCHPICPHYSICAL_OFFSET);
	fnCG_DrawRotatedPicPhysicalW = (CG_DrawRotatedPicPhysicalW_t)(imagebase + function::CG_DRAWROTATEDPICPHYSICALW_OFFSET);
	fnR_TextWidth = (R_TextWidth_t)(imagebase + function::R_TEXTWIDTH_OFFSET);
	fnGetTagPos = (GetTagPos_t)(imagebase + function::GETTAGPOS_OFFSET);
	fnIsIngame = (IsIngame_t)(imagebase + function::ISINGAME_OFFSET);

	regularFont = fnR_RegisterFont(XorString("fonts/main_regular.ttf"), 1);
	materialWhite = fnR_RegisterShader(XorString("white"));

	drawRect = new Rect(0, 0, 0, 0);
	whiteColor = getColorFromRGB(255.f, 255.f, 255.f, 255.f);
	redColor = getColorFromRGB(255.f, 0, 0, 255.f);
	orangeColor = getColorFromRGB(255.f, 135.f, 0, 255.f);
	blackColor = getColorFromRGB(0, 0, 0, 255.f);
	greenColor = getColorFromRGB(0, 255.f, 0, 255.f);
	nameColor = getColorFromRGB(0, 255.f, 125.f, 255.f);
	distanceColor = getColorFromRGB(0, 255.f, 255.f, 255.f);
	itemColor = getColorFromRGB(0, 128.f, 255.f, 255.f);
	radarbgColor = getColorFromRGB(0, 0, 0, 150.f);

	menutextColor = getColorFromRGB(230.f, 105.f, 120.f, 255.f);
	titlebgColor = getColorFromRGB(0, 0, 0, 100);
	titleoutlineColor = getColorFromRGB(255, 220, 0, 100);
	titleColor = getColorFromRGB(255, 220, 0, 220);

	menubgColor = getColorFromRGB(0, 0, 0, 100);
	menuoutlineColor = getColorFromRGB(255, 220, 0, 100);
	menuColor = getColorFromRGB(255, 220, 0, 220);
	menulineColor = getColorFromRGB(255, 255, 255, 100);

	checkboxonColor = getColorFromRGB(145, 240, 145, 220);
	checkboxoffColor = getColorFromRGB(70, 175, 230, 220);

	healthbarColor = getColorFromRGB(0, 0, 0, 100);
	healthgreenColor = getColorFromRGB(0, 255, 0, 220);
	healthorangeColor = getColorFromRGB(255, 158, 53, 220);
	healthredColor = getColorFromRGB(255, 0, 0, 220);

	*(uint64_t*)(imagebase + hook::API_HOOK_OFFSET) = (uint64_t)hLeaveCriticalSection;

	//SetVEH();
	VMProtectEnd();
}