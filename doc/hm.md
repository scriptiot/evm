# evm-qml-littlevgl-qt

### 1. 拷贝hmapp源码到程序build目录

```

cp -rf hmapp/* build-jsfwk-MinGW_64_bit-Debug

```


### 2. 编译ViewModel


#### 2.1 编译
```
npm install
npm run build
```

国内用户

```
npm install -g cnpm
cnpm install
npm run build
```


在viewmodel/build目录会生成

+ **`framework-dev.js`**  
+ **framework-dev.min.js**
+ **framework.js**
+ **framework.min.js**


#### 2.2 解读

+ [globalThis](https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Reference/Global_Objects/globalThis)

    + 在以前，从不同的 JavaScript 环境中获取全局对象需要不同的语句。在 Web 中，可以通过 window、self 或者 frames 取到全局对象，但是在 Web Workers 中，只有 self 可以。在 Node.js 中，它们都无法获取，必须使用 global。

    + 在松散模式下，可以在函数中返回 this 来获取全局对象，但是在严格模式和模块环境下，this 会返回 undefined。 You can also use Function('return this')(), but environments that disable eval(), like CSP in browsers, prevent use of Function in this way.

    + `globalThis 提供了一个标准的方式来获取不同环境下的全局 this  对象（也就是全局对象自身）。`不像 window 或者 self 这些属性，它确保可以在有无窗口的各种环境下正常工作。所以，你可以安心的使用 globalThis，不必担心它的运行环境。为便于记忆，你只需要记住，全局作用域中的 this 就是 globalThis。

    + 编写可在多种环境下工作的可移植 JS 代码是很困难的。每个宿主环境都有稍微不同的对象模型。因此，要访问全局对象，需要在不同的 JS 环境中使用不同的语法。
    
    + `随着globalThis属性的引入，访问全局对象将变得更加简单，并且不再需要检测代码运行的环境。`

```js

function getGlobalObject() {
  if (typeof globalThis !== 'undefined') { return globalThis; }
  if (typeof self !== 'undefined') { return self; }
  if (typeof window !== 'undefined') { return window; }
  if (typeof global !== 'undefined') { return global; }
  throw new Error('cannot find the global object');
};
 
if (typeof getGlobalObject().Promise.allSettled !== 'function') {
  // the Promise.allSettled() method is not available in this environment
}

```

+ 在nodejs中 global 和 globalThis 是一个对象

```
$ node
Welcome to Node.js v12.18.3.
Type ".help" for more information.
> global === globalThis
true
```


+ framework.js 是 viewmodel 编译后语法为es5的viewmodel文件

```
(function (global, factory) {
  typeof exports === 'object' && typeof module !== 'undefined'
    ? (module.exports = factory())
    : typeof define === 'function' && define.amd
    ? define(factory)
    : ((global =
        typeof globalThis !== 'undefined' ? globalThis : global || self),
      (global.ViewModel = factory()));
})(this, function () {
  'use strict';
    ...
   return ViewModel;
})
```

+ framework.js 和 framework-dev.js的区别是 少了Tracing部分逻辑

```
$ diff build/framework.js build/framework-dev.js
325a326,370
>
>   /**
>    * start to trace
>    * @param {number} phase the phase of tracing
>    */
>   function startTracing(phase) {
>     if (typeof profiler !== 'undefined') {
>       profiler.startTracing(phase);
>     }
>   }
>   /**
>    * stop to trace
>    */
>
>   function stopTracing() {
>     if (typeof profiler !== 'undefined') {
>       profiler.stopTracing();
>     }
>   }
>   /**
>    * keep sync with js_profiler.h
>    */
>
>   var PHASE_FWK_EVAL = 5;
>   var PHASE_APP_INIT = 6;
>   var PHASE_INIT_STATE = 7;
>   var PHASE_INIT_DATA_GET_DATA = 8;
>   var PHASE_INIT_DATA_PROXY = 9;
>   var PHASE_INIT_DATA_OBSERVE = 10;
>
>   /*
>    * Copyright (c) 2020 Huawei Device Co., Ltd.
>    * Licensed under the Apache License, Version 2.0 (the "License");
>    * you may not use this file except in compliance with the License.
>    * You may obtain a copy of the License at
>    *
>    *     http://www.apache.org/licenses/LICENSE-2.0
>    *
>    * Unless required by applicable law or agreed to in writing, software
>    * distributed under the License is distributed on an "AS IS" BASIS,
>    * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
>    * See the License for the specific language governing permissions and
>    * limitations under the License.
>    */
>   startTracing(PHASE_APP_INIT);
332a378,379
>     startTracing(PHASE_FWK_EVAL);
>
353a401,402
>
>     stopTracing();
365a415,417
>     startTracing(PHASE_INIT_STATE);
>     startTracing(PHASE_INIT_DATA_GET_DATA);
>
368c420,422
<     } // PHASE_INIT_DATA_GET_DATA
---
>     }
>
>     stopTracing(); // PHASE_INIT_DATA_GET_DATA
369a424
>     startTracing(PHASE_INIT_DATA_PROXY);
372c427,428
<     }); // PHASE_INIT_DATA_PROXY
---
>     });
>     stopTracing(); // PHASE_INIT_DATA_PROXY
374c430,434
<     Subject.of(data); // PHASE_INIT_DATA_OBSERVE // PHASE_INIT_STATE
---
>     startTracing(PHASE_INIT_DATA_OBSERVE);
>     Subject.of(data);
>     stopTracing(); // PHASE_INIT_DATA_OBSERVE
>
>     stopTracing(); // PHASE_INIT_STATE
394a455,456
>
>   stopTracing();
```

+ 编译生成字符buffer.h文件

```
python framework2char.py -j ../viewmodel/build/framework.min.js

```

当前目录下生成framework_min_js.h

```c
#ifndef ACELITE_FRAMEWORK_RAW_BUFFER
#error THIS FILE CAN ONLY BE INCLUDED BY RAW BUFFER CPP
#endif

#ifndef OHOS_ACELITE_FRAMEWORK_MIN_JS_BUFFER
#define OHOS_ACELITE_FRAMEWORK_MIN_JS_BUFFER
const char * const g_frameworkJSBuffer =
    "!function(t,e){'object'==typeof exports&&'undefined'!=typeof module?module.exports=e():'fun"
    "ction'==typeof define&&define.amd?define(e):(t='undefined'!=typeof globalThis?globalThis:t"
    "||self).ViewModel=e()}(this,(function(){'use strict';var t={stack:[],push:function(t){this"
    ".stack.push(t)},pop:function(){return this.stack.pop()},top:function(){return this.stack[t"
    "his.stack.length-1]}},e=function(t){return'object'==typeof t&&null!==t},n=function(t,e,n){"
    "Object.defineProperty(t,e,{enumerable:!1,value:n})};function i(t,e,n,i){this._ctx=t,this._"
    "getter=e,this._fn=n,this._meta=i,this._lastValue=this._get()}function o(e){this._hijacking"
    "=!0,n(e,'__ob__',this),Array.isArray(e)&&function(t){r.forEach((function(e){var i=t[e];n(t"
    ",e,(function(){var n,r=Array.prototype.slice.call(arguments);i.apply(this,r),s.PUSH===e||s"
    ".UNSHIFT===e?n=r:s.SPLICE===e&&(n=r.slice(2)),n&&n.length&&n.forEach(o.of);var c=t.__ob__;"
    "c&&c.notifyParent()}))}))}(e),Object.keys(e).forEach((function(n){return function(e,n,i){v"
    "ar s=e.__ob__;Object.defineProperty(e,n,{enumerable:!0,get:function(){var e=t.top();e&&e.s"
    "ubscribe(s,n);var r=o.of(i);return o.is(r)&&r.setParent(s,n),i},set:function(t){i=t,s.noti"
    "fy(n)}})}(e,n,e[n])}))}i.prototype._get=function(){try{return t.push(this),this._getter.ca"
    "ll(this._ctx)}finally{t.pop()}},i.prototype.update=function(){var t=this._lastValue,n=this"
    "._get(),i=this._ctx,o=this._meta;(n!==t||e(n))&&(this._fn.call(i,n,t,o),this._lastValue=n)"
    "},i.prototype.subscribe=function(t,e){var n=t.attach(e,this);'function'==typeof n&&(this._"
    "detaches||(this._detaches=[]),this._detaches.push(n))},i.prototype.unsubscribe=function(){"
    "var t=this._detaches;if(t)for(;t.length;)t.pop()()},o.of=function(t){return t&&e(t)?t.__ob"
    "__?t.__ob__:new o(t):t},o.is=function(t){return t&&t._hijacking},o.prototype.attach=functi"
    "on(t,e){if(void 0!==t&&e){this._obsMap||(this._obsMap={}),this._obsMap[t]||(this._obsMap[t"
    "]=[]);var n=this._obsMap[t];return n.indexOf(e)<0?(n.push(e),function(){n.splice(n.indexOf"
    "(e),1)}):void 0}},o.prototype.notify=function(t){void 0!==t&&this._obsMap&&this._obsMap[t]"
    "&&this._obsMap[t].forEach((function(t){return t.update()}))},o.prototype.setParent=functio"
    "n(t,e){this._parent=t,this._key=e},o.prototype.notifyParent=function(){this._parent&&this."
    "_parent.notify(this._key)};var s={PUSH:'push',POP:'pop',UNSHIFT:'unshift',SHIFT:'shift',SO"
    "RT:'sort',SPLICE:'splice',REVERSE:'reverse'},r=Object.keys(s).map((function(t){return s[t]"
    "}));function c(t){if(!(this instanceof c))return new c(t);var e=this._vm=this;'[object Obj"
    "ect]'===Object.prototype.toString.call(t)&&Object.keys(t).forEach((function(n){var i=t[n];"
    "'render'===n?e.$render=i:'data'===n?function(t,e){'function'==typeof e&&(e=e.call(t,t));Ob"
    "ject.keys(e).forEach((function(n){return function(t,e,n){Object.defineProperty(t,n,{enumer"
    "able:!1,configurable:!0,get:function(){return e[n]},set:function(t){e[n]=t}})}(t,e,n)})),o"
    ".of(e)}(e,i):'styleSheet'===n?initStyleSheet(i):'function'==typeof i&&(e[n]=i.bind(e))}))}"
    "return c.prototype.$watch=function(t,e,n){return new i(this,t,e,n)},c}));";
#endif

```

### 3. 鸿蒙是如何加载viewmodel模块

+ 加载流程

    + 初始化jerry虚拟机引擎
    + 加载ACE框架内置模块：LoadAceBuiltInModules
        + ConsoleModule::Load();
        + RenderModule::Load();
        + RequireModule::Load();
        + FeaAbilityModule::Load();
        + JsTestModule::Load();
        + TimersModule::Load();
        + PerformaceProfilerModule::Load();
        + AceVersionModule::Load();
    + 加载ViewModel：LoadFramework，使用jerry_eval(jScript, len, JERRY_PARSE_NO_OPTS)执行js代码，js代码中把ViewModel注册到`globalThis`中
    + 加载本地化Local翻译模块LocalModule::Load()，就是在ViewModel内部注册`$t`函数变量

+ 加载Framework

  打开`ace_lite_jsfwk\src\core\context\js_app_environment.cpp` **`77`** 行

  ```c

  void JsAppEnvironment::LoadFramework() const
  {
      size_t len = 0;
      // load framework js/snapshot file to buffer
      const char * const jsFrameworkScript = GetFrameworkRawBuffer(snapshotMode_, len);
      const jerry_char_t *jScript = reinterpret_cast<const jerry_char_t *>(jsFrameworkScript);
      // eval framework to expose
      START_TRACING(FWK_CODE_EVAL);

      jerry_value_t retValue = UNDEFINED;
      if (snapshotMode_) {
          retValue = jerry_exec_snapshot(reinterpret_cast<const uint32_t *>(jScript), len, 0, 1);
      } else {
          retValue = jerry_eval(jScript, len, JERRY_PARSE_NO_OPTS);
      }
      STOP_TRACING();
      bool hasError = jerry_value_is_error(retValue);
      if (hasError) {
          HILOG_ERROR(HILOG_MODULE_ACE, "Failed to load JavaScript framework.");
          ACE_ERROR_CODE_PRINT(EXCE_ACE_FWK_LAUNCH_FAILED, EXCE_ACE_INIT_FWK_FAILED);
          PrintErrorMessage(retValue);
      } else {
          HILOG_INFO(HILOG_MODULE_ACE, "Success to load JavaScript framework.");
      }
      jerry_release_value(retValue);
      Debugger::GetInstance().StartDebugger();
  }

  ```

  + 其中 GetFrameworkRawBuffer 函数就是从framework_min_js.h中读取js文件 g_frameworkBCBuffer

    ```c

    #include "framework_min_js.h"

    const char *GetFrameworkRawBuffer(bool snapshotMode, size_t &bufLen)
    {
        if (snapshotMode) {
            bufLen = sizeof(g_frameworkBCBuffer);
            return g_frameworkBCBuffer;
        } else {
            bufLen = strlen(g_frameworkJSBuffer);
            return g_frameworkJSBuffer;
        }
    }

    ```

+ 加载内置模块LoadAceBuiltInModules 

```
void JsAppEnvironment::LoadAceBuiltInModules() const
{
    ConsoleModule::Load();
    RenderModule::Load();
    RequireModule::Load();
    FeaAbilityModule::Load();
    JsTestModule::Load();
    TimersModule::Load();
    PerformaceProfilerModule::Load();
    AceVersionModule::Load();
}

```

+ 初始化app context

```c

void JsAppEnvironment::InitJsFramework() const
{
    START_TRACING(ENGINE_INIT);
    Srand((unsigned)jerry_port_get_current_time());
    Debugger::GetInstance().SetupJSContext();
    jerry_init(JERRY_INIT_EMPTY);
    STOP_TRACING();
    START_TRACING(FWK_INIT);
#ifdef JSFWK_TEST
    jerry_value_t globalThis = jerry_get_global_object();
    jerry_release_value(jerryx_set_property_str(globalThis, "globalThis", globalThis));
    jerry_release_value(globalThis);
#endif // JSFWK_TEST
    LoadAceBuiltInModules();
    LoadFramework();
    LocalModule::Load();
    STOP_TRACING();
}

```


+ LocalModule加载

  + "ace_lite_jsfwk\src\core\modules\presets\localization_module.cpp"

```c
class LocalModule final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(LocalModule);
    LocalModule() = default;
    ~LocalModule() = default;
    static void Load()
    {
#ifdef FEATURE_LOCALIZATION_MODULE
        LocalizationModule *localizationModule = LocalizationModule::GetInstance();
        localizationModule->Init();
#endif
    }
    static void Clear()
    {
#ifdef FEATURE_LOCALIZATION_MODULE
        LocalizationModule *localization = LocalizationModule::GetInstance();
        localization->Clear();
#endif
    }
};

void LocalizationModule::Init()
{
    jerry_value_t globalContext = jerry_get_global_object();
    const char * const name = "ViewModel";
    jerry_value_t propertyName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(name));
    if (JerryHasProperty(globalContext, propertyName)) {
        // get the prototype of AbilitySlice
        jerry_value_t currentApp = jerry_get_property(globalContext, propertyName);
        jerry_value_t protoType = jerryx_get_property_str(currentApp, "prototype");
        // register $t to the prototype of abilitySlice
        jerry_value_t functionHandle = jerry_create_external_function(Translate);
        const char * const propName = "$t";
        JerrySetNamedProperty(protoType, propName, functionHandle);
        // register $tc to the prototype of abilitySlice
#ifdef LOCALIZATION_PLURAL
        jerry_value_t pluralHandle = jerry_create_external_function(TranslatePlural);
        const char * const pluralFuncName = "$tc";
        JerrySetNamedProperty(protoType, pluralFuncName, pluralHandle);
        jerry_release_value(pluralHandle);
#endif // LOCALIZATION_PLURAL
        ReleaseJerryValue(functionHandle, protoType, currentApp, VA_ARG_END_FLAG);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "app is not create.");
    }
    ReleaseJerryValue(propertyName, globalContext, VA_ARG_END_FLAG);
}


```