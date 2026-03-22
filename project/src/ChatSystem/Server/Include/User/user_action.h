#ifndef USER_ACTION_H
#define USER_ACTION_H

enum class UserAction {
	kSendMessage,

	kDeleteAccount,
	
	kModifyLogin,
	
	kModifyPassword,
	
	kModifyTag,
	
	kDeleteOwnMessage,
	
	kDeleteAnyMessage,
	
	kBanUser,
	
	kManageRoles
};

#endif // USER_ACTION_H
