// Initial version created by "KindOne @ chat.freenode.net".
//
// EXTREMELY CRUDE ZNC module convert NOTICE's to PRIVMSG's.
//
// TODO ...
//
//	Input:
//		- Adding:
//			- Sanitizing checking.
//				- Case insensitive.
//			- Allow adding multiple nicks at once.
//				- /msg *notice add NickServ,ChanServ,MemoServ
//			- Allow adding via Webadmin.
//			- Check for cloaks/vhosts/spoofs.
//
//		- Deleting:
//			- Sanitizing checking.
//				- Make the checking case insensitive.
//			- Allow deleting via Webadmin.
//			- Multi nicks per line.
//				- /msg *notice del NickServ,ChanServ,MemoServ
//		- Allow sorting nicks into A-B-C order.
//
//	Other:
//		- Other things that I might of missed and/or can't think of.
//


#include <znc/Modules.h>
#include <znc/User.h>

class Cnotice : public CModule {

public:
        MODCONSTRUCTOR(Cnotice) {

        }
        virtual ~Cnotice() {}

        void HandleMessage(CNick& Nick, const CString& sMessage) {
                // Flip the NOTICE into PRIVMSG
                PutUser(":" + Nick.GetNickMask() + " PRIVMSG " + GetUser()->GetNick() + " :" + sMessage);
        }

        virtual EModRet OnPrivNotice(CNick& Nick, CString& sMessage) {
                HandleMessage(Nick, sMessage);
                return HALTCORE;
        }
};

template<> void TModInfo<Cnotice>(CModInfo& Info) {
	Info.AddType(CModInfo::NetworkModule);
	//Info.SetWikiPage("notice");
	Info.SetHasArgs(false);
}

NETWORKMODULEDEFS(Cnotice, "Turns NOTICE's into PRIVMSG's.")