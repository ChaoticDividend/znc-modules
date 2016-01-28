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
 *
 * This module was made by Zarthus <zarthus@zarth.us> for personal use, and is not officially endorsed by ZNC in any way.
 * Since 2014-08-19
 * Updated At 2014-12-26
 */

#include <znc/Modules.h>
#include <znc/Chan.h>
#include <znc/User.h>

class CCtcpNotifierMod : public CModule {
public:
    MODCONSTRUCTOR(CCtcpNotifierMod) {
        AddHelpCommand();
        AddCommand("Add", static_cast<CModCommand::ModCmdFunc>(&CCtcpNotifierMod::AddCtcp), "ctcp", "Receive notifications when we reply to CTCP <ctcp>.");
        AddCommand("Delete", static_cast<CModCommand::ModCmdFunc>(&CCtcpNotifierMod::DelCtcp), "ctcp", "Stop receiving notifications when we reply to CTCP <ctcp>.");
        AddCommand("Setup", static_cast<CModCommand::ModCmdFunc>(&CCtcpNotifierMod::LoadCustomUserCtcpResponses), "[unload]", "(Un)load custom CTCP replies configured by the user.");
        AddCommand("Showall", static_cast<CModCommand::ModCmdFunc>(&CCtcpNotifierMod::ShowAllCtcp), "", "Show all incoming CTCPs (toggle)");

        LoadCustomUserCtcpResponses("setup load");
    }
    ~CCtcpNotifierMod() {}

    EModRet OnPrivCTCP(CNick& Nick, CString& sMessage) {
        // if we either want to receive all ctcp messages or GetNV(ctcp) contains something, we notify the user.
        // CTCP ACTIONs are ignored.
        if (IsCtcpNotifier(sMessage.Token(0)))
            PutModule("Replying to CTCP " + sMessage.Token(0) + " by " + Nick.GetNick() + " (" + Nick.GetHostMask() + ").");

        return CONTINUE;
    }

    EModRet OnChanCTCP(CNick& Nick, CChan& Channel, CString& sMessage) {
        if (IsCtcpNotifier(sMessage.Token(0)))
            PutModule("Replying to CTCP " + sMessage.Token(0) + " by " + Nick.GetNick() + " (" + Nick.GetHostMask() + ") to " + Channel.GetName() + ".");

        return CONTINUE;
    }

    void AddCtcp(const CString& sCtcp) {
        // Add a ctcp request from the ctcp queries we're listening to.
        CString sCtcpNotifier = sCtcp.Token(1).AsUpper();

        if (GetNV("ctcp_" + sCtcpNotifier).empty()) {
            SetNV("ctcp_" + sCtcpNotifier, "y");
            PutModule("Added CTCP notifier: " + sCtcpNotifier);
        } else {
            PutModule("CTCP notifier for " + sCtcpNotifier + " was already set.");
        }
    }

    void DelCtcp(const CString& sCtcp) {
        // Remove a ctcp request to the ctcp queries we're listening to.
        CString sCtcpNotifier = sCtcp.Token(1).AsUpper();

        if (!GetNV("ctcp_" + sCtcpNotifier).empty()) {
            DelNV("ctcp_" + sCtcpNotifier);
            PutModule("Deleted CTCP notifier: " + sCtcpNotifier);
        } else {
            PutModule("Could not find CTCP notifier: " + sCtcpNotifier);
        }
    }

    void ShowAllCtcp(const CString& sCtcp) {
        // Toggle between showing all incoming ctcps and just the ones on our notify list.
        if (GetNV("ctcp_showall").empty()) {
            SetNV("ctcp_showall", "y");
            PutModule("Will now show all incoming CTCP replies.");
        } else {
            DelNV("ctcp_showall");
            PutModule("Will no longer show all incoming CTCP replies.");
        }
    }

    void LoadCustomUserCtcpResponses(const CString& sCtcp) {
        // Load all custom CTCP responses the user has.
        bool loading = !sCtcp.Token(1).AsLower().Equals("unload");

        CUser* pUser = GetUser();
        if (pUser == NULL) {
            PutModule("Failed to load custom CTCP responses.");
            return;
        }

        if (loading)
            PutModule("Loading all custom CTCP responses.");
        else
            PutModule("Unloading all custom CTCP responses.");

        const MCString& msCTCPReplies = pUser->GetCTCPReplies();
        for (MCString::const_iterator it = msCTCPReplies.begin(); it != msCTCPReplies.end(); ++it) {
            if (loading) {
                AddCtcp("add " + it->first);
            } else {
                DelCtcp("delete " + it->first);
            }
        }
    }

    bool IsCtcpNotifier(const CString& sCtcp) {
        // Return true if we want all ctcps notified to us, or if "ctcp_"+sCtcp exists.
        // CTCP ACTIONs are the only CTCP query we choose to ignore all the time.
        if (sCtcp.Equals("ACTION"))
            return false;

        return !GetNV("ctcp_showall").empty() || !GetNV("ctcp_" + sCtcp.AsUpper()).empty();
    }
};

template<> void TModInfo<CCtcpNotifierMod>(CModInfo& Info) {
    Info.SetWikiPage("ctcp_notifier");
}

USERMODULEDEFS(CCtcpNotifierMod, "Make ZNC notify you when you receive a CTCP message in your CTCPNotifier list.")