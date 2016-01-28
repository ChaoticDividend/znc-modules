/*
 * Copyright (C) 2004-2009  See the AUTHORS file for details.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <znc/Modules.h>

class CForceIdentifyMsgMod : public CModule {
public:
	MODCONSTRUCTOR(CForceIdentifyMsgMod) {}

	virtual void OnIRCConnected()
	{
		PutIRC("CAP REQ IDENTIFY-MSG");
	}
};

NETWORKMODULEDEFS(CForceIdentifyMsgMod, "Forces sending IDENTIFY-MSG to server")