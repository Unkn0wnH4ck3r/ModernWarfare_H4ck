#pragma once
#include <iostream>
#include <Windows.h>
#include "ida.h"
#define M_PI 3.14159265
class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 operator*(float f)
	{
		return Vector3(x * f, y * f, z * f);
	}
	Vector3 operator/(float f)
	{
		return Vector3(x / f, y / f, z / f);
	}
};
class Vector2
{
public:
	float x;
	float y;

	Vector2() {}
	Vector2(float _X, float _Y) : x(_X), y(_Y) {}

	inline float Length(void) const { return (float)sqrt(x * x + y * y); }

	Vector2 operator+(Vector2 v)
	{
		return Vector2(x + v.x, y + v.y);
	}
	Vector2 operator-(Vector2 v)
	{
		return Vector2(x - v.x, y - v.y);
	}
	Vector2 operator*(float f)
	{
		return Vector2(x * f, y * f);
	}
	Vector2 operator/(float f)
	{
		return Vector2(x / f, y / f);
	}
};
enum BONE_INDEX : int
{
	//头盔
	BONE_POS_HELMET = 8,

	//头到膀胱
	BONE_POS_HEAD = 7,
	BONE_POS_NECK = 6,
	BONE_POS_CHEST = 5,
	BONE_POS_MID = 4,
	BONE_POS_TUMMY = 3,
	BONE_POS_PELVIS = 2,

	//右脚, 与 BONE_POS_PELVIS 连接
	BONE_POS_RIGHT_FOOT_1 = 21,
	BONE_POS_RIGHT_FOOT_2 = 22,
	BONE_POS_RIGHT_FOOT_3 = 23,
	BONE_POS_RIGHT_FOOT_4 = 24,

	//左脚, 与 BONE_POS_PELVIS 连接
	BONE_POS_LEFT_FOOT_1 = 17,
	BONE_POS_LEFT_FOOT_2 = 18,
	BONE_POS_LEFT_FOOT_3 = 19,
	BONE_POS_LEFT_FOOT_4 = 20,

	//右手, 与 BONE_POS_CHEST 连接
	BONE_POS_LEFT_HAND_1 = 13,
	BONE_POS_LEFT_HAND_2 = 14,
	BONE_POS_LEFT_HAND_3 = 15,
	BONE_POS_LEFT_HAND_4 = 16,

	//左手, 与 BONE_POS_CHEST 连接
	BONE_POS_RIGHT_HAND_1 = 9,
	BONE_POS_RIGHT_HAND_2 = 10,
	BONE_POS_RIGHT_HAND_3 = 11,
	BONE_POS_RIGHT_HAND_4 = 12
};
struct playername_s
{
	int index;
	char szName[36];
};
typedef void(*R_AddCmdDrawText_t)(char* text, int maxChars, uint64_t font, uint64_t fontstyle, int fontsize, float x, float y, float x_resize, float y_resize, float scale, float rotation, float* color, uint64_t unk1, bool unk2, uint64_t unk3, bool unk4, uint64_t unk5);
typedef __int64(*R_RegisterFont_t)(const char* name, int imageTrack);
typedef __int64(*R_RegisterShader_t)(const char* name);
typedef __int64(*R_RegisterTag_t)(const char* name);
typedef void(*CL_DrawStretchPicPhysical_t)(float* rect, float a2, float a3, float a4, int a5, float* color, __int64 material);
typedef void(*CG_DrawRotatedPicPhysicalW_t)(float x, float y, float w, float h, float a5, float a6, float a7, float a8, float angle, bool a10, bool a11, float* color, __int64 material);
typedef __int32(*R_TextWidth_t)(char* text, int maxChars, __int64 font);
typedef void(__fastcall* GetDecryptedPos_t)(uint64_t entity, Vector3* decpos);
typedef bool(*GetTagPos_t)(uint64_t entity, uint64_t bonearray, int boneid, float* pos);
typedef bool(__stdcall* IsIngame_t)(void);

namespace Entity_0x318
{
	DWORD ENCRYPT_PTR_OFFSET = 0x10BF6C18;
	DWORD REVERSED_ADDRESS = 0x47630D6;
}
namespace Bone_0x150
{
	DWORD ENCRYPT_PTR_OFFSET = 0xEDD7608;
	DWORD REVERSED_ADDRESS = 0x4763240;
}
namespace ClientInfo_0x3588
{
	DWORD ENCRYPT_PTR_OFFSET = 0x10BF7F38;
	DWORD REVERSED_ADDRESS = 0x4763161;
}
namespace UserCmd
{
	DWORD ENCRYPT_PTR_OFFSET = 0x1083CEB0;
	DWORD REVERSED_ADDRESS = 0x47631A0;
}
namespace XorPos_0xC
{
	DWORD ENTITY_XOR1_OFFSET = 0x170;
	DWORD ENTITY_XOR2_OFFSET = 0x178;
	DWORD ENTITY_XORVALUE_OFFSET = 0x180;

	DWORD ITEM_XOR1_OFFSET = 0x150;
	DWORD ITEM_XOR2_OFFSET = 0x158;
	DWORD ITEM_XORVALUE_OFFSET = 0x160;

	DWORD REVERSED_ADDRESS = 0x63C5D08;
}
namespace hook
{
	DWORD API_HOOK_OFFSET = 0x14B66B14;
	DWORD LOBBY_RET_OFFSET = 0x2574D66;
	DWORD GAME_RET_OFFSET = 0x2F8F097;
}
namespace function
{
	DWORD R_ADDCMDDRAWTEXT_OFFSET = 0x364EC30;
	DWORD R_REGISTERFONT_OFFSET = 0x35CEFA0;
	DWORD R_REGISTERSHADER_OFFSET = 0x3630F40;
	DWORD CL_DRAWSTRETCHPICPHYSICAL_OFFSET = 0x364DC30;
	DWORD CG_DRAWROTATEDPICPHYSICALW_OFFSET = 0x364E290;
	DWORD R_TEXTWIDTH_OFFSET = 0x35CF370;
	DWORD GETTAGPOS_OFFSET = 0x29370B0;
	DWORD ISINGAME_OFFSET = 0x2757DA0;
}
namespace offsets
{
	DWORD REFDEF_OFFSET = 0x10BF9C70;
	DWORD NAME_ARRAY_OFFSET = 0x10C05528;
	DWORD NAME_LIST_OFFSET = 0x4C70;//Unk

	DWORD ENT_TYPE_OFFSET = 0x5C;
	DWORD ENT_STANCE_OFFSET = 0xB4;
	DWORD ENT_VAILD_OFFSET = 0x4C;
	DWORD ENT_INDEX_OFFSET = 0x54;
	DWORD ENT_SKELETON = 0x150;
	DWORD ENT_MAXSIZE_OFFSET = 0x348;

	DWORD REF_WIDTH_OFFSET = 0x8;
	DWORD REF_HEIGHT_OFFSET = 0xC;
	DWORD REF_FOVX_OFFSET = 0x10;
	DWORD REF_FOVY_OFFSET = 0x14;
	DWORD REF_MATRIX_OFFSET = 0x24;

	DWORD CI_VIEWORIGIN_OFFSET = 0x457C8;
	DWORD CI_LOCAL_OFFSET = 0x174B8;
	DWORD CI_VIEWANGLE_OFFSET = 0x22F4;
	DWORD CI_RECOIL_OFFSET = 0x46E0;

	DWORD EI_BASE_OFFSET = 0x97A38;
	DWORD EI_INDEX_OFFSET = 0x0;
	DWORD EI_VAILD_OFFSET = 0x4;
	DWORD EI_TEAMID_OFFSET = 0xC;
	DWORD EI_ANGLE_OFFSET = 0x9B8;
	DWORD EI_MAXSIZE_OFFSET = 0x39D0;

	DWORD ITEM_MASK_OFFSET = 0x10D86D20;
	DWORD ITEM_BASE_OFFSET = 0x10D50120;
	DWORD ITEM_POS_OFFSET = 0x190;
	DWORD ITEM_MAXSIZE_OFFSET = 0x248;

	DWORD INDEX_ARRAY_OFFSET = 0xEDD6240;
	DWORD BONE_MAXSIZE_OFFSET = 0x150;

	DWORD INPUT_SEED_OFFSET = 0x8590;
	DWORD INPUT_ANGLE_OFFSET = 0x8598;

	DWORD BRDATA_INSTANCE_OFFSET = 0x10C05528;
	DWORD BRDATA_BASE_OFFSET = 0x4C70;
	DWORD BRDATA_HEALTH = 0x4C;

	DWORD SCREENSIZE_OFFSET = 0x1084D100;
	DWORD KEYSTATE_OFFSET = 0x1084C654;
}
namespace decrypt
{
	uint64_t general_obfuscate(uint64_t encryptedPtr, uint64_t* curr_decrypt_key)
	{
		encryptedPtr = (__int64)encryptedPtr * curr_decrypt_key[0];
		encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0x1D);
		encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0x3A);
		encryptedPtr = (__int64)encryptedPtr * curr_decrypt_key[1];
		encryptedPtr = (encryptedPtr >> 0x07) ^ encryptedPtr;
		encryptedPtr = (encryptedPtr >> 0x0E) ^ encryptedPtr;
		encryptedPtr = (encryptedPtr >> 0x1C) ^ encryptedPtr;
		encryptedPtr = (encryptedPtr >> 0x38) ^ encryptedPtr;
		encryptedPtr = (__int64)encryptedPtr * curr_decrypt_key[2];
		encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0x1B);
		encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0x36);
		encryptedPtr = (__int64)encryptedPtr * curr_decrypt_key[3];
		encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0x0B);
		encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0x16);
		encryptedPtr = encryptedPtr ^ (encryptedPtr >> 0x2C);
		return encryptedPtr;
	}

	uint64_t* decrypt_key_for_entity(uint64_t imagebase)
	{
		static uint64_t key[16][4];

		static bool Empty = true;
		if (Empty)
		{
			uint64_t reversedAddr, lastKey;
			reversedAddr = *(uint64_t*)(imagebase + Entity_0x318::REVERSED_ADDRESS);
			lastKey = *(uint64_t*)(_byteswap_uint64(reversedAddr) + 0xB);

			key[0][0] = 0x6BCA71CA233A5B3F;
			key[0][1] = 0x407FF2D8DB843333;
			key[0][2] = lastKey;
			key[0][3] = 0xCDC89CA85DFC61CF;

			key[1][0] = 0x666CA3583A80F3AB;
			key[1][1] = 0xFEF03739D81566A3;
			key[1][2] = lastKey;
			key[1][3] = 0x29D28A22BC323F05;

			key[2][0] = 0x5D53D77C08AAB319;
			key[2][1] = 0x8F1F8CD2E372D31B;
			key[2][2] = 0x25B2DC16BC03CC77;
			key[2][3] = lastKey;

			key[3][0] = 0x153F5A8209182E29;
			key[3][1] = lastKey;
			key[3][2] = 0x475BCAB3F0074CBD;
			key[3][3] = 0xF4CAEA123315C21B;

			key[4][0] = 0x76C817DA4F39C4AF;
			key[4][1] = 0xBE7730603A5C3361;
			key[4][2] = lastKey;
			key[4][3] = 0xD90875C082C743E5;

			key[5][0] = 0x809C94CFF45952EB;
			key[5][1] = lastKey;
			key[5][2] = 0xD6D2639BC8067C43;
			key[5][3] = 0x54D70B3D7A97D36F;

			key[6][0] = 0xDFB133032222F8BD;
			key[6][1] = 0x30A0091FDED0A32D;
			key[6][2] = lastKey;
			key[6][3] = 0x4121BA06F3FB9411;

			key[7][0] = 0x4BBFEC2C18470A1B;
			key[7][1] = lastKey;
			key[7][2] = 0x92566C4CA9952365;
			key[7][3] = 0x6450CDBFF4DBB651;

			key[8][0] = 0xD8C8C8CE9F6C42F;
			key[8][1] = 0xF5C61CBF7D48656D;
			key[8][2] = 0xA7C9ED3D37E951E9;
			key[8][3] = lastKey;

			key[9][0] = lastKey;
			key[9][1] = 0xAF3B2C9F5B466EFB;
			key[9][2] = 0x4CBA036722479BF;
			key[9][3] = 0x6E3CDDB31BAD87DD;

			key[10][0] = 0x6A77E0E467E5761;
			key[10][1] = 0xDF5A2CF9161612F1;
			key[10][2] = lastKey;
			key[10][3] = 0x71355ADAF1865D31;

			key[11][0] = 0x6801814482EC78B;
			key[11][1] = lastKey;
			key[11][2] = 0xC8A74E1A8F040981;
			key[11][3] = 0x8489152C64BF039F;

			key[12][0] = lastKey;
			key[12][1] = 0xA9A38490F070EBB1;
			key[12][2] = 0x749F2F3C75F8F19B;
			key[12][3] = 0x9736F02A9373ECCF;

			key[13][0] = 0xC95845F5CFE29B61;
			key[13][1] = lastKey;
			key[13][2] = 0x7949BBE7D1D6CFCF;
			key[13][3] = 0xD49CC74FE430D711;

			key[14][0] = lastKey;
			key[14][1] = 0x813826F648415D79;
			key[14][2] = 0x3B64BAB99542FDF7;
			key[14][3] = 0xEBB53F0FF1B5ACA3;

			key[15][0] = lastKey;
			key[15][1] = 0xB9270D9A96A7DB43;
			key[15][2] = 0x964F2A270099C15D;
			key[15][3] = 0x8ECF3A376F3C5827;
			Empty = false;
		}

		return (uint64_t*)key;
	}

	uint64_t* decrypt_key_for_bone(uint64_t imagebase)
	{
		static uint64_t key[16][4];

		static bool Empty = true;
		if (Empty)
		{
			uint64_t reversedAddr, lastKey;

			reversedAddr = *(uint64_t*)(imagebase + Bone_0x150::REVERSED_ADDRESS);
			lastKey = *(uint64_t*)(_byteswap_uint64(reversedAddr) + 0x9);

			key[0][0] = 0x11BBC1CDC9D50D83;
			key[0][1] = lastKey;
			key[0][2] = 0x3EEFFC69D1C10225;
			key[0][3] = 0x4A9F71A20EB85963;

			key[1][0] = lastKey;
			key[1][1] = 0x2F780267AC72C9FD;
			key[1][2] = 0x9261C9FBA3CADCF1;
			key[1][3] = 0x1F36052EF40E1061;

			key[2][0] = 0x75FBAB7E0EE74BAD;
			key[2][1] = 0xC10B24AA91F1D275;
			key[2][2] = lastKey;
			key[2][3] = 0x7E0249F5E770A065;

			key[3][0] = 0x69D25F5CA44D4755;
			key[3][1] = 0x66D4B5B12F14AEB;
			key[3][2] = lastKey;
			key[3][3] = 0xEB96DF4EAFF1C4D9;

			key[4][0] = 0xFE5701A2735B2021;
			key[4][1] = 0x981E0C23FF7D323D;
			key[4][2] = lastKey;
			key[4][3] = 0xBB852E3C647058C5;

			key[5][0] = 0x525453F1850A72AD;
			key[5][1] = 0x35ADD67F49CA7FA3;
			key[5][2] = 0xD350FF2B4C5FA645;
			key[5][3] = lastKey;

			key[6][0] = 0x75049A1A51C044E9;
			key[6][1] = 0xC8247C931731F5FD;
			key[6][2] = lastKey;
			key[6][3] = 0xBF68E3446CADB39D;

			key[7][0] = lastKey;
			key[7][1] = 0xF0BFA91859AF409;
			key[7][2] = 0xE08262C89A58AF0B;
			key[7][3] = 0x5F4AE9AB966240E7;

			key[8][0] = 0x34C2EE6491045BC3;
			key[8][1] = 0x864CEBAB8436B547;
			key[8][2] = lastKey;
			key[8][3] = 0x1D07F5BCE333AADD;

			key[9][0] = 0x982F21FBEDBBFF67;
			key[9][1] = lastKey;
			key[9][2] = 0x7FF0A869975086CB;
			key[9][3] = 0x4D4B641B835243A3;

			key[10][0] = 0x55AC85E325115F6B;
			key[10][1] = 0xDFD0CD35462997B1;
			key[10][2] = lastKey;
			key[10][3] = 0x3DA5238C1D080551;

			key[11][0] = lastKey;
			key[11][1] = 0x5FCFD5A7CB4972D7;
			key[11][2] = 0x32CF4430C5A4ACCF;
			key[11][3] = 0x1A4519711362357B;

			key[12][0] = 0x814C9A33126A8595;
			key[12][1] = 0x6179878CEFFBC823;
			key[12][2] = lastKey;
			key[12][3] = 0xE761425299EA0ED5;

			key[13][0] = 0x3D20F51E470FFACF;
			key[13][1] = 0x95AAE36DB24F5AE1;
			key[13][2] = lastKey;
			key[13][3] = 0x15A4A410A1B92069;

			key[14][0] = 0x6CC6A4018670E079;
			key[14][1] = 0xB9D657DDFF2020E1;
			key[14][2] = lastKey;
			key[14][3] = 0x5A1737A5690BEBB7;

			key[15][0] = 0xF4DC309B387FFA51;
			key[15][1] = 0xFD2C6B0A4F9F795;
			key[15][2] = 0x92076877D63B653F;
			key[15][3] = lastKey;
			Empty = false;
		}

		return (uint64_t*)key;
	}

	uint64_t* decrypt_key_for_clientinfo(uint64_t imagebase)
	{
		static uint64_t key[4];

		static bool Empty = true;
		if (Empty)
		{
			uint64_t reversedAddr, LastKey;
			reversedAddr = *(uint64_t*)(imagebase + ClientInfo_0x3588::REVERSED_ADDRESS);
			LastKey = *(uint64_t*)(_byteswap_uint64(reversedAddr) + 0x1B);
			key[0] = 0xD2F4B49C63080843;
			key[1] = 0x27C65D7E5008A707;
			key[2] = LastKey;
			key[3] = 0xFF1E6202EF30250F;
			Empty = false;
		}
		return (uint64_t*)key;
	}

	uint64_t* decrypt_key_for_inputptr(uint64_t imagebase)
	{
		static uint64_t key[16][4];

		static bool Empty = true;
		if (Empty)
		{
			uint64_t reversedAddr, lastKey;

			reversedAddr = *(uint64_t*)(imagebase + UserCmd::REVERSED_ADDRESS);
			lastKey = *(uint64_t*)(_byteswap_uint64(reversedAddr) + 0x17);

			key[0][0] = lastKey;
			key[0][1] = 0x6D567327A314AEA7;
			key[0][2] = 0xD6080B7BA1697A15;
			key[0][3] = 0x364327CC6D517C89;

			key[1][0] = lastKey;
			key[1][1] = 0x936CB95B88CD4AA5;
			key[1][2] = 0xD992CEEA05A64BD7;
			key[1][3] = 0x23BBDB7150046883;

			key[2][0] = 0xA80D29796C83A617;
			key[2][1] = 0x9C4D22471C10EC9F;
			key[2][2] = lastKey;
			key[2][3] = 0x4519A1198D807937;

			key[3][0] = 0x97921F422032A749;
			key[3][1] = 0xB54F075602D62D11;
			key[3][2] = 0x76FCA71C1D906725;
			key[3][3] = lastKey;

			key[4][0] = 0x61D3014CCFCB8FA9;
			key[4][1] = 0x8B08C5A5282AC483;
			key[4][2] = 0xB39EB12C8A35486F;
			key[4][3] = lastKey;

			key[5][0] = 0x994FDAA405A5A1EF;
			key[5][1] = 0x1435F10E02CD1077;
			key[5][2] = 0x8B46866C495AD02B;
			key[5][3] = lastKey;

			key[6][0] = 0x530D8B332F4F67F1;
			key[6][1] = lastKey;
			key[6][2] = 0x9035F6805708D7;
			key[6][3] = 0xD12104FF5DAE6297;

			key[7][0] = lastKey;
			key[7][1] = 0x47EC96CC48AF122D;
			key[7][2] = 0x563EA393C1EF729D;
			key[7][3] = 0xB809A2EDFF78381;

			key[8][0] = lastKey;
			key[8][1] = 0xDC145DDB94346781;
			key[8][2] = 0xFC73ADC77100A403;
			key[8][3] = 0x6EB9807EFE004E6F;

			key[9][0] = lastKey;
			key[9][1] = 0xA5B44105AFCE511F;
			key[9][2] = 0x8E62DD3BF1498591;
			key[9][3] = 0xED026DD77B26BB83;

			key[10][0] = 0x2BC15B2375DBB589;
			key[10][1] = 0x4848124F67B8D0DF;
			key[10][2] = lastKey;
			key[10][3] = 0xDC3E77679CDF3F9B;

			key[11][0] = 0x68A9D11CA4E59E7F;
			key[11][1] = 0x51E8BF12EFBA7489;
			key[11][2] = lastKey;
			key[11][3] = 0xD911E6F5C8A81285;

			key[12][0] = 0xD64FDE6225CE2E1D;
			key[12][1] = lastKey;
			key[12][2] = 0xB8701B76E30A0A25;
			key[12][3] = 0x68F398EA252A2A41;

			key[13][0] = 0xD823BCC5083412EF;
			key[13][1] = 0x1E826BA0546E005B;
			key[13][2] = lastKey;
			key[13][3] = 0xC673185ED1A03CE7;

			key[14][0] = 0x82A67216A865935B;
			key[14][1] = 0x4DF2B22C2BAD30BF;
			key[14][2] = 0x66141FC386BC061;
			key[14][3] = lastKey;

			key[15][0] = 0xFBBD4C1744A34C8D;
			key[15][1] = lastKey;
			key[15][2] = 0x765C0A55AC8CE6D9;
			key[15][3] = 0x491FA5E716BAE081;
			Empty = false;
		}

		return (uint64_t*)key;
	}

	uint64_t decrypt_for_entity(uint64_t imagebase, DWORD pebindex)
	{
		uint64_t encryptedPtr = *(uint64_t*)(imagebase + Entity_0x318::ENCRYPT_PTR_OFFSET);
		if (encryptedPtr) {
			encryptedPtr = *(uint64_t*)(encryptedPtr + 0x10);
			return general_obfuscate(encryptedPtr, &decrypt_key_for_entity(imagebase)[pebindex << 2]);
		}
		return 0;
	}

	uint64_t decrypt_for_bone(uint64_t imagebase, DWORD pebindex)
	{
		uint64_t encryptedPtr = *(uint64_t*)(imagebase + Bone_0x150::ENCRYPT_PTR_OFFSET);
		if (encryptedPtr) {
			return general_obfuscate(encryptedPtr, &decrypt_key_for_bone(imagebase)[pebindex << 2]);
		}
		return 0;
	}

	uint64_t decrypt_for_clientinfo(uint64_t imagebase)
	{
		uint64_t encryptedPtr = *(uint64_t*)(imagebase + ClientInfo_0x3588::ENCRYPT_PTR_OFFSET);
		if (encryptedPtr) {
			return general_obfuscate(encryptedPtr, decrypt_key_for_clientinfo(imagebase));
		}
		return 0;
	}

	uint64_t decrypt_for_inputptr(uint64_t imagebase, DWORD pebindex)
	{
		uint64_t encryptedPtr = *(uint64_t*)(imagebase + UserCmd::ENCRYPT_PTR_OFFSET);
		if (encryptedPtr) {
			return general_obfuscate(encryptedPtr, &decrypt_key_for_inputptr(imagebase)[pebindex << 2]);
		}
		return 0;
	}

	Vector3 decrypt_for_entitypos(uint64_t imagebase, uint64_t entity)
	{
		Vector3 validLocation = Vector3(0, 0, 0);
		uint64_t pDecFunc = (*(uint32_t*)(imagebase + XorPos_0xC::REVERSED_ADDRESS) ^ (~(entity + XorPos_0xC::ENTITY_XOR1_OFFSET)) ^ *(uint64_t*)(entity + XorPos_0xC::ENTITY_XOR2_OFFSET));
		if (pDecFunc)
			reinterpret_cast<GetDecryptedPos_t>(pDecFunc)(entity + XorPos_0xC::ENTITY_XORVALUE_OFFSET, &validLocation);
		return validLocation;
	}

	Vector3 decrypt_for_itempos(uint64_t imagebase, uint64_t entity)
	{
		Vector3 validLocation = Vector3(0, 0, 0);
		uint64_t pDecFunc = (*(uint32_t*)(imagebase + XorPos_0xC::REVERSED_ADDRESS) ^ (~(entity + XorPos_0xC::ITEM_XOR1_OFFSET)) ^ *(uint64_t*)(entity + XorPos_0xC::ITEM_XOR2_OFFSET));
		if (pDecFunc)
			reinterpret_cast<GetDecryptedPos_t>(pDecFunc)(entity + XorPos_0xC::ITEM_XORVALUE_OFFSET, &validLocation);
		return validLocation;
	}

	uint64_t decrypt_for_refdef(uint64_t imagebase)
	{
		uint64_t refdef = NULL;
		int low = *(uint32_t*)(imagebase + offsets::REFDEF_OFFSET + 0x8) ^ (unsigned __int64)(imagebase + offsets::REFDEF_OFFSET);
		int high = *(uint32_t*)(imagebase + offsets::REFDEF_OFFSET + 0x8) ^ (unsigned __int64)(imagebase + offsets::REFDEF_OFFSET + 0x4);
		LODWORD(refdef) = *(uint32_t*)(imagebase + offsets::REFDEF_OFFSET) ^ (low * (low + 2));
		HIDWORD(refdef) = *(uint32_t*)(imagebase + offsets::REFDEF_OFFSET + 0x4) ^ (high * (high + 2));
		return refdef;
	}

}
class Rect
{
public:
	float lt_x, lt_y, rt_x, rt_y, rb_x, rb_y, lb_x, lb_y;
	Rect(float x, float y, float w, float h) {
		this->lt_x = x; this->lt_y = y; this->rt_x = x + w; this->rt_y = y; this->rb_x = x + w; this->rb_y = y + h; this->lb_x = x; this->lb_y = y + h;
	}
	float* toFloat();
	void getRect(float x, float y, float w, float h);
};
float* Rect::toFloat()
{
	return (float*)this;
}
void Rect::getRect(float x, float y, float w, float h)
{
	this->lt_x = x; this->lt_y = y; this->rt_x = x + w; this->rt_y = y; this->rb_x = x + w; this->rb_y = y + h; this->lb_x = x; this->lb_y = y + h;
}

class Color
{
	float r, g, b, t;
public:
	Color(float _r, float _g, float _b, float _t) { this->r = _r; this->g = _g; this->b = _b; this->t = _t; }
	float* toFloat();
};
float* Color::toFloat()
{
	return (float*)this;
}
Color* getColorFromRGB(float r, float g, float b, float t)
{
	return new Color((1.f / 255.f) * r, (1.f / 255.f) * g, (1.f / 255.f) * b, (1.f / 255.f) * t);
}