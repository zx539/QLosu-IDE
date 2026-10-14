/**
 * QLosu WebAssembly 入口（QLosu 原创代码）
 *
 * 功能：将 QLosu 量子编程平台（Losu v2 内核 + Qufu 声明式量子框架 + QuEST 模拟器）
 *       封装为浏览器 / Node.js 可直接调用的 WASM 导出函数。
 *
 * 导出接口：
 *   - qlosu_version()           获取平台版本信息
 *   - qlosu_run(code, len)      编译并运行一段 Losu 量子脚本
 *
 * 说明：脚本中通过 `import qufu` 引入量子框架，qufu.losu 以内嵌文件形式
 *       打包进 WASM（LOSU_PATH=/），运行时由内核按模块导入规则自动加载。
 */

#include "losu_api.h"
#include "losu_base.h"

#include <emscripten/emscripten.h>

#include <stdlib.h>
#include <string.h>

/**
 * @brief 获取 QLosu 平台版本信息
 * @return 版本字符串（静态内存，无需释放）
 */
EMSCRIPTEN_KEEPALIVE
const char *
qlosu_version (void)
{
  return "QLosu 1.0.0 (Losu v2 + Qufu + QuEST 4.2.0)";
}

/**
 * @brief 编译并运行一段 Losu 量子脚本（从字符串加载，无需文件系统输入）
 * @param code 脚本源码（UTF-8）
 * @param len  源码字节长度
 * @return 0 成功；非 0 失败
 */
EMSCRIPTEN_KEEPALIVE
int
qlosu_run (const char *code, int len)
{
  if (code == NULL || len <= 0)
    return -1;

  /* qufu 量子框架以内嵌模块形式打包进 WASM（见 qlosu_mod.c） */
  losu_vm_t vm = losu_api_vm_init (0);
  if (vm == NULL)
    return -2;

  struct losu_object_t script_main = { 0 };
  int ret = losu_api_io_loadstream (vm, &script_main, code, (size_t)len,
                                    "<qlosu-web>");
  if (ret != 0)
    {
      losu_api_vm_deinit (vm);
      return ret;
    }

  struct losu_object_t thobj
      = losu_api_obj_thread_new (vm, script_main, 1024);
  losu_api_obj_thread_setstatus (vm, thobj,
                                 losu_obj_thread_status_enum_t_ready);
  losu_api_vm_mainloop (vm);
  losu_api_vm_deinit (vm);
  return 0;
}
