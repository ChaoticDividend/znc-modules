// Initial version created by "KindOne @ chat.freenode.net".
//
// EXTREMELY CRUDE ZNC module to join channels on invite.
//
// TODO ...
//
//	Input:
//		- Adding:
//			- Sanutizing checking.
//				- Case insensitive.
//			- Only allow valid CHANTYPE=
//			- Honor CHANNELLEN=
//			- Allow adding multiple channels at once.
//				- /msg *autojoin ##kindone,##foobar
//			- Allow adding via Webadmin.
//			- Option to only allow certain nicks to send invites.
//				- Check for cloaks/vhosts/spoofs.
//
//		- Deleting:
//			- Sanitizing checking.
//				- Make the checking case insensitive.
//			- Allow deleting via Webadmin.
//			- Multi channels per line.
//				- /msg *autojoin del ##kindone,##foobar
//		- Allow sorting nicks into A-B-C order.
//


#include <znc/Chan.h>
#include <znc/IRCNetwork.h>
#include <znc/Modules.h>


using std::vector;

class Cautojoin : public CModule {
public:

        MODCONSTRUCTOR(Cautojoin) {

        }

        virtual ~Cautojoin() {}

        virtual EModRet OnInvite(const CNick& Nick, const CString& sChan) {
                PutIRC("JOIN " + sChan);
                return CONTINUE;
        }
};

NETWORKMODULEDEFS(Cautojoin, "Autojoin on /invite.")