---

---

---

Changelog: V).16.7-a
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