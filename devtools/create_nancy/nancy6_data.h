/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NANCY6DATA_H
#define NANCY6DATA_H

#include "types.h"

const GameConstants _nancy6Constants ={
	36,												// numItems
	456,											// numEventFlags
	{	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,			// genericEventFlags
		11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
		21, 22, 23, 24, 25, 26, 27, 28, 29, 30 },
	16,												// numCursorTypes
	4000,											// logoEndAfter
	32												// wonGameFlagID
};

const Common::Array<GameLanguage> _nancy6LanguagesOrder = {
	GameLanguage::kEnglish,
	GameLanguage::kRussian
};

const Common::Array<Common::Array<ConditionalDialogue>> _nancy6ConditionalDialogue = {
{	// Joanna, 17 responses
	{	0, 1050, "NJR50",
		{ { kEv, 304, true }, { kEv, 364, false }, { kEv, 202, false } } },
	{	0, 1051, "NJR51",
		{ { kEv, 304, true }, { kEv, 371, false }, { kEv, 184, false } } },
	{	0, 1052, "NJR52",
		{ { kEv, 304, true }, { kEv, 361, false }, { kEv, 198, false } } },
	{	0, 1053, "NJR53",
		{ { kEv, 304, true }, { kEv, 398, true }, { kEv, 347, false }, { kEv, 194, false } } },
	{	0, 1054, "NJR54",
		{ { kEv, 241, true }, { kEv, 231, true }, { kEv, 188, false } } },
	{	0, 1055, "NJR55",
		{ { kEv, 391, true }, { kEv, 189, false }, { kEv, 239, false } } },
	{	0, 1057, "NJR57",
		{ { kEv, 239, true }, { kEv, 201, false }, { kEv, 415, false } } },
	{	0, 1058, "NJR58",
		{ { kEv, 321, true }, { kEv, 186, false } } },
	{	0, 1060, "NJR60",
		{ { kEv, 305, true }, { kEv, 209, false }, { kEv, 344, false } } },
	{	0, 1063, "NJR63",
		{ { kEv, 212, true }, { kEv, 148, false }, { kEv, 200, false } } },
	{	0, 1064, "NJR64",
		{ { kEv, 391, true }, { kEv, 191, false }, { kEv, 239, false } } },
	{	0, 1065, "NJR65",
		{ { kEv, 241, true }, { kEv, 240, true }, { kEv, 190, false } } },
	{	0, 1068, "NJR68",
		{ { kEv, 310, true }, { kEv, 39, false }, { kEv, 197, false } } },
	{	0, 1069, "NJR69",
		{ { kEv, 420, true }, { kEv, 118, false }, { kEv, 203, false } } },
	{	0, 1070, "NJR70",
		{ { kEv, 231, true }, { kEv, 247, true }, { kEv, 205, false } } },
	{	0, 1071, "NJR71",
		{ { kEv, 247, true }, { kEv, 204, false } } },
	{	0, 1072, "NTS60",
		{ { kEv, 397, true }, { kEv, 154, false }, { kEv, 187, false } } }
},
{	// Taylor, 10 responses
	{	0, 1250, "NTA50",
		{ { kEv, 45, true }, { kEv, 416, false } } },
	{	0, 1253, "NTA53",
		{ { kEv, 231, true }, { kEv, 407, false } } },
	{	0, 1254, "NTA54",
		{ { kEv, 231, true }, { kEv, 409, false } } },
	{	0, 1256, "NTA56",
		{ { kEv, 243, true }, { kEv, 411, false }, { kIn, 2, true } } },
	{	0, 1257, "NTA57",
		{ { kEv, 239, true }, { kEv, 415, false } } },
	{	0, 1258, "NTA58",
		{ { kEv, 148, true }, { kEv, 417, false } } },
	{	0, 1260, "NTS60",
		{ { kEv, 397, true }, { kEv, 154, false }, { kEv, 408, false } } },
	{	0, 1262, "NTS62",
		{ { kEv, 212, true }, { kEv, 424, false } } },
	{	0, 1263, "NTS63",
		{ { kEv, 310, true }, { kEv, 39, false }, { kEv, 413, false } } },
	{	0, 1264, "NTA64",
		{ { kEv, 243, true }, { kEv, 375, false }, { kEv, 410, false }, { kIn, 2, false } } }
},
{	// Alejandro, 9 responses
	{	0, 1350, "NAR50",
		{ { kEv, 241, true }, { kEv, 231, true }, { kEv, 40, false } } },
	{	0, 1351, "NAR51",
		{ { kEv, 391, true }, { kEv, 42, false } } },
	{	0, 1353, "NAR53",
		{ { kEv, 255, true }, { kEv, 36, false } } },
	{	0, 1361, "NAR61",
		{ { kEv, 288, true }, { kEv, 384, false }, { kEv, 47, false } } },
	{	0, 1362, "NAR62",
		{ { kEv, 310, true }, { kEv, 41, false } } },
	{	0, 1370, "NAR70",
		{ { kEv, 148, true }, { kEv, 44, false } } },
	{	0, 1371, "NAR71",
		{ { kEv, 43, true }, { kEv, 37, false } } },
	{	0, 1372, "NAR72",
		{ { kEv, 186, true }, { kEv, 242, true }, { kEv, 38, false } } },
	{	0, 1308, "NAR79",
		{ { kEv, 231, true }, { kEv, 45, false } } }
},
{	// Henrik (in lab), 13 responses + 1 repeat
	{	0, 1450, "NHL50",
		{ { kEv, 406, true }, { kEv, 185, true }, { kEv, 151, false } } },
	{	0, 1452, "NHL52",
		{ { kEv, 401, true }, { kEv, 364, false }, { kEv, 167, false } } },
	{	0, 1453, "NJR51",
		{ { kEv, 304, true }, { kEv, 371, false }, { kEv, 149, false } } },
	{	0, 1454, "NHL54",
		{ { kEv, 304, true }, { kEv, 361, false }, { kEv, 163, false } } },
	{	0, 1455, "NHL55",
		{ { kEv, 175, true }, { kEv, 398, true }, { kEv, 161, false } } },
	{	0, 1456, "NHL56",
		{ { kEv, 208, true }, { kEv, 175, false } } },
	{	0, 1456, "NHL56",
		{ { kEv, 300, true }, { kEv, 175, false } } },
	{	0, 1457, "NHL57",
		{ { kEv, 245, true }, { kEv, 172, false } } },
	{	0, 1459, "NHL59",
		{ { kEv, 302, false }, { kEv, 166, false }, { kIn, 5, true } } },
	{	0, 1460, "NHL60",
		{ { kEv, 241, true }, { kEv, 171, false } } },
	{	0, 1462, "NHL62",
		{ { kEv, 241, true }, { kEv, 156, false } } },
	{	0, 1464, "NHL64",
		{ { kEv, 285, true }, { kEv, 157, false } } },
	{	0, 1465, "NHL65",
		{ { kEv, 303, true }, { kEv, 176, false } } },
	{	0, 1466, "NHL66",
		{ { kEv, 241, true }, { kEv, 159, false } } }
},
{	// Henrik (in hospital), 14 responses
	{	0, 1550, "NHH50",
		{ { kEv, 307, true }, { kEv, 243, true }, { kEv, 177, false } } },
	{	0, 1551, "NHH51",
		{ { kEv, 309, true }, { kEv, 158, false } } },
	{	0, 1552, "NHH52",
		{ { kEv, 284, true }, { kEv, 150, false } } },
	{	0, 1553, "NHH53",
		{ { kEv, 397, true }, { kEv, 345, false }, { kEv, 169, false } } },
	{	0, 1554, "NHH54",
		{ { kEv, 147, true }, { kEv, 374, false } } },
	{	0, 1555, "NHH55",
		{ { kEv, 147, true }, { kEv, 374, true }, { kEv, 165, false } } },
	{	0, 1556, "NHH56",
		{ { kEv, 311, true }, { kEv, 173, false } } },
	{	0, 1558, "NHH58",
		{ { kEv, 420, true }, { kEv, 244, false }, { kEv, 168, false } } },
	{	0, 1562, "NHH62",
		{ { kEv, 93, true }, { kEv, 152, false } } },
	{	0, 1563, "NHH63",
		{ { kEv, 420, true }, { kEv, 147, true }, { kEv, 170, false } } },
	{	0, 1565, "NHH65",
		{ { kEv, 242, true }, { kEv, 153, false } } },
	{	0, 1567, "NHH67",
		{ { kEv, 310, true }, { kEv, 39, false }, { kEv, 162, false } } },
	{	0, 1568, "NHH68",
		{ { kEv, 307, true }, { kEv, 174, false } } },
	{	0, 1569, "NSH188",
		{ { kEv, 403, true }, { kEv, 335, false } } }
},
{	// Bess & George, 14 responses + 2 repeats
	{	0, 1620, "NBG20a",
		{ { kEv, 232, true }, { kEv, 231, true }, { kEv, 63, false } } },
	{	0, 1621, "NBG21a",
		{ { kEv, 246, true }, { kEv, 255, false }, { kEv, 69, false }, { kEv, 67, false } } },
	{	0, 1623, "NBG23",
		{ { kEv, 69, true }, { kEv, 247, true }, { kEv, 255, false }, { kEv, 67, false } } },
	{	0, 1624, "NBG24",
		{ { kEv, 232, true }, { kEv, 246, true }, { kEv, 71, false }, { kEv, 148, false } } },
	{	0, 1626, "NBG26",
		{ { kEv, 75, true }, { kEv, 79, false }, { kEv, 148, false } } },
	{	0, 1627, "NBG27",
		{ { kEv, 405, true }, { kEv, 36, true }, { kEv, 147, false }, { kEv, 78, false } } },
	{	0, 1628, "NBG28",
		{ { kEv, 40, true }, { kEv, 147, false }, { kEv, 68, false } } },
	{	0, 1628, "NBG28",
		{ { kEv, 47, true }, { kEv, 147, false }, { kEv, 68, false } } },
	{	0, 1628, "NBG28",
		{ { kEv, 245, true }, { kEv, 147, false }, { kEv, 68, false } } },
	{	0, 1629, "NBG29",
		{ { kEv, 41, true }, { kEv, 288, true }, { kEv, 115, false }, { kEv, 388, false }, { kEv, 64, false } } },
	{	0, 1630, "NBG30",
		{ { kEv, 420, true }, { kEv, 80, false } } },
	{	0, 1631, "NBG31",
		{ { kEv, 120, true }, { kEv, 348, false }, { kEv, 81, false } } },
	{	0, 1632, "NBG32",
		{ { kEv, 243, true }, { kEv, 375, false }, { kEv, 76, false } } },
	{	0, 1633, "NBG33",
		{ { kEv, 284, true }, { kEv, 77, false } } },
	{	0, 1670, "NBG70",
		{ { kEv, 232, true }, { kEv, 73, false }, { kDi, 2, true } } },
	{	0, 1671, "NBG70",
		{ { kEv, 232, true }, { kDi, 0, true } } }
},
{	// Frank & Jo, 6 responses
	{	0, 1720, "NFJ20",
		{ { kEv, 239, true }, { kEv, 165, false }, { kEv, 108, false } } },
	{	0, 1723, "NFJ23",
		{ { kEv, 112, true }, { kIn, 4, true } } },
	{	0, 1724, "NFJ24",
		{ { kEv, 306, true }, { kEv, 311, true }, { kEv, 340, false }, { kEv, 111, false } } },
	{	0, 1725, "NFJ25",
		{ { kEv, 310, true }, { kEv, 334, false }, { kEv, 114, false } } },
	{	0, 1770, "NFJ70",
		{ { kEv, 237, true }, { kEv, 109, false } } },
	{	0, 1771, "NFJ70",
		{ { kEv, 237, true }, { kEv, 109, true } } }
}
};

const Common::Array<Goodbye> _nancy6Goodbyes = {
	{ "NJR90", { { { 1090, 1092, 1093, 1094, 1095 }, {}, NOFLAG } } },	// Joanna
	{ "NTA90", { { { 1290, 1291, 1292, 1293 }, {}, NOFLAG } } },		// Taylor
	{ "NAR90", { { { 1390, 1391, 1393 }, {}, NOFLAG } } },				// Alejandro
	{ "NHL90", { { { 1490, 1491, 1492, 1493 }, {}, NOFLAG } } },		// Henrik (lab)
	{ "NHH90", { { { 1590, 1591 }, {}, NOFLAG } } },					// Henrik (hospital)
	{ "NBG90", { { { 1690, 1691, 1692, 1693, 1694 }, {}, NOFLAG } } },	// Bess & George
	{ "NFJ90", { { { 1790, 1791, 1792, 1793 }, {}, NOFLAG } } }			// Frank & Jo
};

const Common::Array<const char *> _nancy6TelephoneRinging = {
	"ringing...<n><e>", // English
	"\x46\xf6\xe9\xe2\xf4...  <n><e>" // Russian
};

const Common::Array<const char *> _nancy6EmptySaveStrings = {
	"Nothing Saved Here",	// English
	"- - - - -         "	// Russian
};

const Common::Array<const char *> _nancy6EventFlagNames = {
	"EV_Generic0",
	"EV_Generic1",
	"EV_Generic2",
	"EV_Generic3",
	"EV_Generic4",
	"EV_Generic5",
	"EV_Generic6",
	"EV_Generic7",
	"EV_Generic8",
	"EV_Generic9",
	"EV_Generic10",
	"EV_Generic11",
	"EV_Generic12",
	"EV_Generic13",
	"EV_Generic14",
	"EV_Generic15",
	"EV_Generic16",
	"EV_Generic17",
	"EV_Generic18",
	"EV_Generic19",
	"EV_Generic20",
	"EV_Generic21",
	"EV_Generic22",
	"EV_Generic23",
	"EV_Generic24",
	"EV_Generic25",
	"EV_Generic26",
	"EV_Generic27",
	"EV_Generic28",
	"EV_Generic29",
	"EV_Generic30",
	"EV_TimeForEndgame",
	"EV_PlayerWonGame",
	"EV_StopPlayerScrolling",
	"EV_Easter_Eggs",
	"EV_AR_Available",
	"EV_AR_Said_Alibi",
	"EV_AR_Said_Baseball",
	"EV_AR_Said_Cinnabar",
	"EV_AR_Said_Coatl",
	"EV_AR_Said_Expensive",
	"EV_AR_Said_Favor",
	"EV_AR_Said_Nahuatl",
	"EV_AR_Said_Pillowhead",
	"EV_AR_Said_Poor_Henrik",
	"EV_AR_Said_Provenance",
	"EV_AR_Said_Theft",
	"EV_AR_Said_Tip",
	"EV_Assembled_Chaco",
	"EV_Assembled_Copan",
	"EV_Assembled_Cortega",
	"EV_Assembled_Landa",
	"EV_Assembled_Pacal",
	"EV_Assembled_Rutherford",
	"EV_BBall_GameWon",
	"EV_BBall_High",
	"EV_BBall_Hoop",
	"EV_BBall_Low",
	"EV_BBall_Position1",
	"EV_BBall_Position2",
	"EV_BBall_Position3",
	"EV_BBall_Position4",
	"EV_BBall_VeryLow",
	"EV_BG_Said_AR",
	"EV_BG_Said_Bargain",
	"EV_BG_Said_Champion",
	"EV_BG_Said_Difficult",
	"EV_BG_Said_Eye_Out",
	"EV_BG_Said_Financial",
	"EV_BG_Said_Foil",
	"EV_BG_Said_Hardys",
	"EV_BG_Said_Monolith",
	"EV_BG_Said_Mope",
	"EV_BG_Said_Negative",
	"EV_BG_Said_Pacal",
	"EV_BG_Said_Pacal_Theft",
	"EV_BG_Said_Poppy",
	"EV_BG_Said_Roadster",
	"EV_BG_Said_Stranger",
	"EV_BG_Said_Third_Degree",
	"EV_BG_Said_Trademark",
	"EV_BG_Said_Vindicate",
	"EV_Blue_Said_Message",
	"EV_Blue_Said_Visiting_Hours",
	"EV_Calendar_Comment01",
	"EV_Calendar_Comment02",
	"EV_Calendar_Comment03",
	"EV_CalendarInProgress",
	"EV_Called_Mack",
	"EV_Called_Silvio",
	"EV_Card_In_Reader",
	"EV_Chaco_Package_Available",
	"EV_Chaco_Said_Foam_Core",
	"EV_Chaco_Said_Glyph",
	"EV_Chaco_Said_Henrik_Left",
	"EV_Chaco_Said_Henrik_Terms",
	"EV_Chaco_Said_Jade_Carving",
	"EV_Chaco_Said_North",
	"EV_Chaco_Said_Photo",
	"EV_Chaco_Said_Red_Hand",
	"EV_Chaco_Said_Replica",
	"EV_Chaco_Said_TS",
	"EV_Checked_Contract",
	"EV_Checked_Logograph_Numbers",
	"EV_Checked_Narrations",
	"EV_Checked_Pottery",
	"EV_Disk_In_Drive",
	"EV_FJ_Said_Ace",
	"EV_FJ_Said_Amnesia",
	"EV_FJ_Said_Code",
	"EV_FJ_Said_Collaborate",
	"EV_FJ_Said_First_King",
	"EV_FJ_Said_Glolite",
	"EV_FJ_Said_Myst",
	"EV_FJ_Said_Smugglers",
	"EV_FR_Said_Case_Closed",
	"EV_FR_Said_Heat_Seeker",
	"EV_FR_Said_Joanna_Back",
	"EV_FR_Said_Prudence_Number",
	"EV_FR_Said_Redeem",
	"EV_FR_Said_Take_Charge",
	"EV_FR_Said_Theft_Message",
	"EV_GB_Said_Trigger",
	"EV_Ham_Dead",
	"EV_Ham_Fixed",
	"EV_Ham_On",
	"EV_Heard_BL_Message",
	"EV_Heard_Noise01",
	"EV_Heard_Noise02",
	"EV_Heard_Noise03",
	"EV_Heard_Noise04",
	"EV_Heard_Noise05",
	"EV_Heard_Noise06",
	"EV_Heard_Noise07",
	"EV_Heard_Noise08",
	"EV_Heard_Noise09",
	"EV_Heard_Noise10",
	"EV_Heard_Noise11",
	"EV_Heard_Noise12",
	"EV_Heard_Noise13",
	"EV_Heard_Noise14",
	"EV_Heard_Noise15",
	"EV_Heard_Noise16",
	"EV_Heard_Noise17",
	"EV_Heard_Noise18",
	"EV_Heard_Noise19",
	"EV_Heard_Noise20",
	"EV_HH_Confessed_Theft",
	"EV_HH_Konk",
	"EV_HH_Said_Addenda",
	"EV_HH_Said_Afterlife",
	"EV_HH_Said_Chaco1",
	"EV_HH_Said_Chaco2",
	"EV_HH_Said_Cinnabar",
	"EV_HH_Said_Coatimundi",
	"EV_HH_Said_Copan",
	"EV_HH_Said_Guesswork",
	"EV_HH_Said_Ham1",
	"EV_HH_Said_Ham2",
	"EV_HH_Said_Homebase",
	"EV_HH_Said_Hurricane",
	"EV_HH_Said_Logograph_Numbers",
	"EV_HH_Said_Nahuatl",
	"EV_HH_Said_Narrations",
	"EV_HH_Said_Oozing_Hives",
	"EV_HH_Said_Pacal_Tomb",
	"EV_HH_Said_Password",
	"EV_HH_Said_Pot",
	"EV_HH_Said_Prudence",
	"EV_HH_Said_Quiz",
	"EV_HH_Said_Red_Hand",
	"EV_HH_Said_Riddle",
	"EV_HH_Said_Silvio",
	"EV_HH_Said_Six_Keys",
	"EV_HH_Said_Smuggling",
	"EV_HH_Said_Sonny",
	"EV_HH_Said_SpectroX",
	"EV_HH_Said_TS",
	"EV_HH_Saw_Pot",
	"EV_HHH_Available",
	"EV_HHL_Available",
	"EV_Insulted_PR",
	"EV_JR_Available",
	"EV_JR_Returned",
	"EV_JR_Said_Addenda",
	"EV_JR_Said_Chaco",
	"EV_JR_Said_Cinnabar",
	"EV_JR_Said_Coatimundi",
	"EV_JR_Said_Curator",
	"EV_JR_Said_Fish_Food",
	"EV_JR_Said_Giddyup",
	"EV_JR_Said_Handprint",
	"EV_JR_Said_Investigate",
	"EV_JR_Said_Landa_Code",
	"EV_JR_Said_Logograph_Numbers",
	"EV_JR_Said_Magician",
	"EV_JR_Said_Monolith",
	"EV_JR_Said_Nahuatl",
	"EV_JR_Said_Narrations",
	"EV_JR_Said_Nightmare",
	"EV_JR_Said_No_Henrik",
	"EV_JR_Said_No_Photo",
	"EV_JR_Said_Pottery",
	"EV_JR_Said_Prudence",
	"EV_JR_Said_Security",
	"EV_JR_Said_Stealing",
	"EV_JR_Said_Suspended_Message",
	"EV_JR_Said_Tasklist",
	"EV_JR_Said_Tempest",
	"EV_JR_Said_Temple",
	"EV_JR_Said_Unpack",
	"EV_JR_Said_What_Next",
	"EV_JR_Said_Yellow_Death",
	"EV_JR_Suspended",
	"EV_KeyPuz_Tried_InHole",
	"EV_KeyPuz_Tried_Turn",
	"EV_KeyPuzCurrTryE",
	"EV_KeyPuzCurrTryN",
	"EV_KeyPuzCurrTryS",
	"EV_KeyPuzCurrTryW",
	"EV_KeyPuzSideE",
	"EV_KeyPuzSideN",
	"EV_KeyPuzSideS",
	"EV_KeyPuzSideW",
	"EV_Mack_Said_At_Lunch",
	"EV_Mack_Said_Distributor",
	"EV_Mack_Said_Joanna",
	"EV_Mack_Said_Last_Week",
	"EV_Mack_Said_No_Prob",
	"EV_Mack_Said_Pick_Up",
	"EV_Mack_Said_Sulfide",
	"EV_Met_AR",
	"EV_Met_BG",
	"EV_Met_BL",
	"EV_Met_Blue",
	"EV_Met_Chaco",
	"EV_Met_Daddle",
	"EV_Met_FJ",
	"EV_Met_FR",
	"EV_Met_HH_Hospital",
	"EV_Met_HH_Lab",
	"EV_Met_JR",
	"EV_Met_Mack",
	"EV_Met_Poppy",
	"EV_Met_Prudence",
	"EV_Met_Silvio",
	"EV_Met_TS_Monolith",
	"EV_Met_TS_Office",
	"EV_Mold_Set",
	"EV_Mold_Setting",
	"EV_ND_Said_Pwd",
	"EV_Opened_Chaco_Package",
	"EV_Opened_Drawer",
	"EV_Opened_Rutherford_Package",
	"EV_Opened_Smuggler_Package",
	"EV_Pacal_Theft",
	"EV_PD_Said_Art",
	"EV_PD_Said_Carving",
	"EV_PD_Said_Cookie",
	"EV_Placed_Calendar",
	"EV_Placed_Knob",
	"EV_Placed_LogoA",
	"EV_Placed_LogoB",
	"EV_Placed_Pacal_Photo",
	"EV_Placed_PotteryA",
	"EV_Placed_PotteryB",
	"EV_Placed_Riddle",
	"EV_Placed_Threat",
	"EV_PR_Said_Babble",
	"EV_PR_Said_Red_Hand",
	"EV_PR_Said_Send_Copy",
	"EV_Prudence_Ready",
	"EV_Rutherford_Package_Available",
	"EV_Said_Air01",
	"EV_Said_Air02",
	"EV_Said_Air03",
	"EV_Said_Art",
	"EV_Said_Light01",
	"EV_Said_Light02",
	"EV_Said_Logo1",
	"EV_Saw_CCCC_Number",
	"EV_Saw_Contacts",
	"EV_Saw_Donor_Plaque",
	"EV_Saw_Exhibit",
	"EV_Saw_Exhibit_Burial_Customs",
	"EV_Saw_Ham",
	"EV_Saw_HH_Note",
	"EV_Saw_JR_Alarm_Cine",
	"EV_Saw_JR_Gone_Note",
	"EV_Saw_Koko",
	"EV_Saw_Landa_List",
	"EV_Saw_Level3_Quiz",
	"EV_Saw_Locked_Tube",
	"EV_Saw_Pacal_Theft",
	"EV_Saw_Periodic_Table",
	"EV_Saw_Provenance_Docs",
	"EV_Saw_Red_Hand",
	"EV_Saw_Riddle",
	"EV_Saw_Rutherford_Article",
	"EV_Saw_Smuggler_Codes",
	"EV_Saw_Sonny",
	"EV_Saw_Sonny_Logo_Notes",
	"EV_Saw_Sonny_Notes",
	"EV_Saw_SpectroX",
	"EV_Saw_Tasklist",
	"EV_Saw_Temple",
	"EV_Saw_Translation",
	"EV_Saw_Travel_Notes",
	"EV_Saw_Zap",
	"EV_Saw_Zap_Contacts",
	"EV_Saw_Zap_Honduras",
	"EV_Saw_Zap_Key_Notes",
	"EV_Saw_Zap_North",
	"EV_Saw_Zap_South",
	"EV_Saw_Zap_West",
	"EV_Silvio_Said_Beat_It",
	"EV_Smuggler_Package_Available",
	"EV_Solved_Basketball",
	"EV_Solved_Bul",
	"EV_Solved_Calendar",
	"EV_Solved_Cinnabar",
	"EV_Solved_Elements",
	"EV_Solved_Eye",
	"EV_Solved_God_Match",
	"EV_Solved_God_MatchA",
	"EV_Solved_God_MatchB",
	"EV_Solved_God_MatchC",
	"EV_Solved_God_MatchD",
	"EV_Solved_God_MatchE",
	"EV_Solved_God_MatchF",
	"EV_Solved_God_MatchG",
	"EV_Solved_God_MatchH",
	"EV_Solved_God_MatchI",
	"EV_Solved_God_MatchJ",
	"EV_Solved_Ham",
	"EV_Solved_HH_Pwd",
	"EV_Solved_KeyPuzE",
	"EV_Solved_KeyPuzN",
	"EV_Solved_KeyPuzS",
	"EV_Solved_KeyPuzW",
	"EV_Solved_Lamat",
	"EV_Solved_Landa_Code",
	"EV_Solved_Leche",
	"EV_Solved_Level1",
	"EV_Solved_Level2",
	"EV_Solved_Level3",
	"EV_Solved_Logograph_Match",
	"EV_Solved_Logograph_Numbers",
	"EV_Solved_LogographA",
	"EV_Solved_LogographB",
	"EV_Solved_LogographC",
	"EV_Solved_LogographD",
	"EV_Solved_LogographE",
	"EV_Solved_LogographF",
	"EV_Solved_LogographG",
	"EV_Solved_LogographH",
	"EV_Solved_LogographI",
	"EV_Solved_LogographJ",
	"EV_Solved_Maze",
	"EV_Solved_Mercury",
	"EV_Solved_Mold",
	"EV_Solved_Narrations",
	"EV_Solved_Nightmare",
	"EV_Solved_Number_Channel",
	"EV_Solved_Pottery_Task",
	"EV_Solved_Smuggler_Station",
	"EV_Solved_Sonny_Pwd",
	"EV_Solved_SpectroX",
	"EV_Solved_Sulphur",
	"EV_Spectro_On",
	"EV_Tasks_Done",
	"EV_Took_Addenda",
	"EV_Took_Calendar",
	"EV_Took_Cookie",
	"EV_Took_Copan",
	"EV_Took_Cortega",
	"EV_Took_Diskette",
	"EV_Took_Jade",
	"EV_Took_Key_Rutherford",
	"EV_Took_Knob",
	"EV_Took_Landa",
	"EV_Took_LogoA",
	"EV_Took_LogoB",
	"EV_Took_Mold",
	"EV_Took_Pacal_Carving",
	"EV_Took_Passport",
	"EV_Took_PotteryA",
	"EV_Took_PotteryB",
	"EV_Took_Provenance",
	"EV_Took_Riddle",
	"EV_Took_Rutherford_Key",
	"EV_Took_Threat",
	"EV_Took_Tube",
	"EV_Took_Zap",
	"EV_Tried_Cabinet",
	"EV_Tried_God_Match",
	"EV_Tried_HH_Pwd",
	"EV_Tried_Level3",
	"EV_Tried_Logograph_Numbers",
	"EV_Tried_Maze",
	"EV_Tried_Narrations",
	"EV_Tried_Pottery_Task",
	"EV_Tried_Sonny_Pwd",
	"EV_Tried_Zap",
	"EV_TS_Available",
	"EV_TS_Said_Alibi",
	"EV_TS_Said_Appraised",
	"EV_TS_Said_Bully",
	"EV_TS_Said_Coatimundi",
	"EV_TS_Said_Commission",
	"EV_TS_Said_Cookie",
	"EV_TS_Said_Creative",
	"EV_TS_Said_Functions",
	"EV_TS_Said_Nahuatl",
	"EV_TS_Said_Office",
	"EV_TS_Said_Photo",
	"EV_TS_Said_Pig",
	"EV_TS_Said_Poor_Henrik",
	"EV_TS_Said_Poppy",
	"EV_TS_Said_Prudence",
	"EV_TS_Said_Red_Hand",
	"EV_TS_Said_Scoundrel",
	"EV_TS_Said_Sick",
	"EV_TS_Said_Socks",
	"EV_TS_Said_Yellow_Death",
	"EV_Zap_In_Drive",
	"EV_Empty1426",
	"EV_Empty1427",
	"EV_Empty1428",
	"EV_Empty1429",
	"EV_Empty1430",
	"EV_Empty1431",
	"EV_Empty1432",
	"EV_Empty1433",
	"EV_Empty1434",
	"EV_Empty1435",
	"EV_Empty1436",
	"EV_Empty1437",
	"EV_Empty1438",
	"EV_Empty1439",
	"EV_Empty1440",
	"EV_Empty1441",
	"EV_Empty1442",
	"EV_Empty1443",
	"EV_Empty1444",
	"EV_Empty1445",
	"EV_Empty1446",
	"EV_Empty1447",
	"EV_Empty1448",
	"EV_Empty1449",
	"EV_Empty1450",
	"EV_Empty1451",
	"EV_Empty1452",
	"EV_Empty1453",
	"EV_Empty1454",
	"EV_TBD"
};

// Patch notes:
// This is the official patch from HeR, which is supposed
// to fix a freeze while talking to Bess & George, or at
// the monolith. I've not experienced said issues, and the
// patched scene is too big for me to inspect, so this is
// untested (though it most likely works fine)

const Common::Array<const char *> nancy6PatchSrcFiles {
	"S1671.cif"
};

const Common::Array<PatchAssociation> nancy6PatchAssociations {
	{ { "softlocks_fix", "true" }, { "S1671" } }
};

#endif // NANCY6DATA_H
