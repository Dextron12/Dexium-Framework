---

---
---
### Changelog: V0.16.8-a

- Fixed a `Shader` issue, where if a shader had successfully compiled, it wouldn't update its `isCompiled` to reflect so. This prevented other systems that relied on probing a successful compilation to continue where early exiting.
- Separated `Texture.hpp` into a proper target `.cpp` for definitions to avoid `STB_IMAGE` cross linkage issues throughout the project. Honestly not sure how I didn't run into this sooner.
- Implemeted the `Opaque` renderer system, so any commands submitted under the opaque setting should render.
- Observed a memory leak through KDE(System Monitor) on the process. Tried using `Valgrind` to detect the leak, but it mostly reports on GL junk and I have a suspicion the leak is not from the engine itself but the testing code to test the renderer.
	- Opened *Bug 1* for this issue.
---

### Changelog: V0.16.7-a
-  Removed implicit global state `gwinMasks` a bit-field enum class that allowed setting aGL buffer to be cleared. 
	- Now `clearColor` has clear responsiblities and **does not** invoke `glClear` or `glClearColor` but instead instructs the renderer to do so on `Renderer::flush()`
	- I believe the only other global logic in existence at the moment are:
		- Window resize event signal `sig_onWindowResize` and the engine state itself `EngineState::get()`.
- Added a runtime check for Shaders to ensure they have successfully compiled `Shader::isCompiled()` returns a `bool` indicating such
	- It will be used in `Renderer::Submit` to align with insufficient state not being pushed to the renderer.
	- Also be sued in `Renderer:Flush()` to ensure even on a hot-reloaded shader, it it fails, it cannot be rendered or actioned(the command)

---
### Changelog: V0.16.6-a
- Implemented MonoClock. Provides a monotonic clock functionality independent from GLFwGetTime(), but does sue `std::chrono::steady_clock()` and should be noted that this may not deliver the best performance on Windows OS machines.

- Implemented a `localTime` clock that uses `chrono::system_clock` behind the scenes to capture the HW time and convert it to local time(to respect DST etc). It also provides the functionality to provide the Hour in 12hr time or 24hr time with a handy `am_pm()` to also deduce and provide the correct tag for the time of day.
- Used the `localTime` clock to finish the Logger File sink, leaving only the `DevTerm` sink to be incorporated.
---

Changelog: ???

Think its safe to say any prior changelogs have been lost to history due to a certain maintainer accidentally running `sudo rm -rf` on the docs folder, after a rather long debugging night....

Lesson painfully learnt, now I will look toward maintaining better changelogs and version updates moving forward.

I believe the `legacy` branch on the [repo](https://github.com/Dextron12/Dexium-Framework/tree/legacy1) along with the `main` branch will also provide good insight into versioning history in their commit comments.

But what's important is that these changelogs are dated afetr the Dexium-Framework structure re-write and will contian the most up to date changelogs