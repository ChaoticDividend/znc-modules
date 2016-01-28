/*
 * Copyright (C) 2004-2014 ZNC, see the NOTICE file for details.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <znc/Modules.h>
#include <znc/Nick.h>

class CKvircMod : public CModule {
	CString m_sAvatar;
	enum EGender {
		NONE = 0,
		MALE = 1,
		FEMALE = 2,
		BOT = 3
	} m_eGender;
	CString m_sColor;
public:
	MODCONSTRUCTOR(CKvircMod) {}

	virtual bool OnLoad(const CString& sArgs, CString& sMessage) {
		m_sAvatar = GetNV("Avatar");
		m_eGender = static_cast<EGender>(GetNV("Gender").ToUInt());
		m_sColor = GetNV("Color");
		return true;
	}

	virtual EModRet OnIRCRegistration(CString& sPass, CString& sNick, CString& sIdent, CString& sRealName) {
		int nCode = m_eGender;
		if (!m_sAvatar.empty()) {
			nCode |= 4;
		}
		sRealName = "\x03" + CString(nCode) + "\x0F" + (m_sColor.empty() ? "" : "\x03" + m_sColor + "\x0F") + sRealName;
		return CONTINUE;
	}

	virtual EModRet OnPrivCTCP(CNick& Nick, CString& sMessage) {
		if (!m_sAvatar.empty() && "AVATAR" == sMessage) {
			PutIRC("NOTICE " + Nick.GetNick() + " :\001AVATAR " + m_sAvatar + "\001");
			return HALT;
		}
		return CONTINUE;
	}

	virtual EModRet OnChanCTCP(CNick& Nick, CChan& Channel, CString& sMessage) {
		if (!m_sAvatar.empty() && "AVATAR" == sMessage) {
			PutIRC("NOTICE " + Nick.GetNick() + " :\001AVATAR " + m_sAvatar + "\001");
			return HALT;
		}
		return CONTINUE;
	}

	virtual void OnModCommand(const CString& sCommand) {
		CString sCmdName = sCommand.Token(0).AsLower();
		if ("gender" == sCmdName) {
			CString sGender = sCommand.Token(1).AsLower();
			if (sGender.empty()) {
				PutModule("Usage: gender male|female|bot|none");
				return;
			}
			switch (sGender[0]) {
				case 'n':
					m_eGender = NONE;
					break;
				case 'm':
					m_eGender = MALE;
					break;
				case 'f':
				case 'w':
					m_eGender = FEMALE;
					break;
				case 'b':
					m_eGender = BOT;
					break;
				default:
					PutModule("Usage: gender male|female|bot|none");
					return;
			}
			SetNV("Gender", CString(m_eGender));
			PutModule("Gender set");
		} else if ("avatar" == sCmdName) {
			m_sAvatar = sCommand.Token(1, true);
			SetNV("Avatar", m_sAvatar);
			if (m_sAvatar.empty()) {
				PutModule("Avatar unset");
			} else {
				PutModule("Avatar set");
			}
		} else if ("color" == sCmdName) {
			m_sColor = sCommand.Token(1);
			SetNV("Color", m_sColor);
			if (m_sColor.empty()) {
				PutModule("Nickcolor unset");
			} else {
				PutModule("Nickcolor set to " + m_sColor);
			}
		} else if ("show" == sCmdName) {
			if (m_sAvatar.empty()){
				PutModule("Avatar not set");
			} else {
				PutModule("Avatar: " + m_sAvatar);
			}
			switch (m_eGender) {
				case NONE:
					PutModule("Gender: none");
					break;
				case MALE:
					PutModule("Gender: male");
					break;
				case FEMALE:
					PutModule("Gender: female");
					break;
				case BOT:
					PutModule("Gender: bot");
					break;
			}
			if (m_sColor.empty()) {
				PutModule("Nickcolor not set");
			} else {
				PutModule("Nickcolor: " + m_sColor);
			}
		} else {
			PutModule("Commands: gender, avatar, color, show");
		}
	}
};

template<> void TModInfo<CKvircMod>(CModInfo& Info) {
	Info.SetWikiPage("kvirc");
	Info.AddType(CModInfo::UserModule);
}

MODULEDEFS(CKvircMod, "Allows to set your gender and avatar, so KVIrc users will see it.")
