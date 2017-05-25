#ifndef _ERRORS_H
#define _ERRORS_H

namespace Error {
	const int LTFSDM_GENERAL_ERROR               = -1;
	const int LTFSDM_OK                          =  0;
	const int LTFSDM_COMM_ERROR                  =  1001;
	const int LTFSDM_ATTR_FORMAT                 =  1002;
	const int LTFSDM_FS_CHECK_ERROR              =  1003;
	const int LTFSDM_FS_ADD_ERROR                =  1004;
	const int LTFSDM_TAPE_EXISTS_IN_POOL         =  1005;
	const int LTFSDM_TAPE_NOT_EXISTS_IN_POOL     =  1006;
	const int LTFSDM_POOL_EXISTS                 =  1007;
	const int LTFSDM_POOL_NOT_EXISTS             =  1008;
	const int LTFSDM_TAPE_NOT_EXISTS             =  1009;
	const int LTFSDM_POOL_NOT_EMPTY              =  1010;
	const int LTFSDM_WRONG_POOLNUM               =  1011;
	const int LTFSDM_NOT_ALL_POOLS_EXIST         =  1012;
	const int LTFSDM_DRIVE_BUSY                  =  1013;

	const int LTFSDM_ALREADY_FORMATTED           =  1050;
	const int LTFSDM_WRITE_PROTECTED             =  1051;
	const int LTFSDM_TAPE_STATE_ERR              =  1052;
	const int LTFSDM_INACCESSIBLE                =  1054;
};

#endif /* _ERRORS_H */
