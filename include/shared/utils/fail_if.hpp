#include <cstdio>
#include <cstring>

#define __FILENAME__(file) (std::strrchr(file, '/') ? std::strrchr(file, '/') + 1 : file)

#define FAIL_IF(cond)						                                             \
	do {								                                                         \
		if (cond) {						                                                      \
         printf("[FAIL_IF]||%s||%s||%d||%s\n", __FILENAME__(__FILE__), __FUNCTION__, __LINE__, #cond); \
			goto fail;			                                                            \
		}							                                                            \
	} while (0)

#define FAIL_IF_MSG(cond, MSG)						                                             \
	do {								                                                         \
		if (cond) {						                                                      \
         printf("[FAIL_IF]||%s||%s||%d||%s||%s\n", __FILENAME__(__FILE__), __FUNCTION__, __LINE__, #cond, MSG); \
			goto fail;			                                                            \
		}							                                                            \
	} while (0)

#define FAIL_IF_SILENT(cond)						                                    \
	do {								                                                         \
		if (cond) {						                                                      \
			goto fail;			                                                            \
		}							                                                            \
	} while (0)
