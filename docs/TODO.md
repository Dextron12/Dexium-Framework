
- [x] Remove `gwinmasks` and use `Renderer::clear`, `Renderer::clearColor` instead 🔽 ➕ 2026-02-27 ✅ 2026-03-10
- [ ] Implement `Renderer` being able to use multiple texture units/slots and eficiently sort between them. ⏫ ➕ 2026-02-27 
- [ ] Implement a `Renderer` based Text fn 🔼 ➕ 2026-02-27 
- [ ] Implement delayed destruction of `Renderer` managed objects.
	Can be done by creating an `EngineState::destroyObj` that pushes it to a vector, then in `windowContext::endFrame` we iterate through the vec and explicitly call delete on the obj
	Might need to be templated to so above.

	- [ ] Add the `transparent` renderComm loop into `Renderer::flush()` 🛫 2026-03-02 
- [ ] `ResourcePool` an entirely **OPTIONAL** system that can load, optionallly cache and handle meta data like hot-reloading.
	- [ ] Perhaps the `ResourcePool` system can work on a resource handle system in this form:
		```
		struct ResHandle{
			T* ptr;
			ResType type;
			
			std::string label;
			
		};
		```










- [x] **Bug: 1**: `Renderer::flush()` in `Opaque` `RenderQueue` is likely causing a small memory leak. Unsure if this is because we are creating a `RenderCommand` each frame (which likely is caused by the poor initializer of this struct) or somehting entirely else. #Debug #Investigate ✅ 2026-03-10
	This Bug has been throuroughly investigated, using `Valgrind`, `HTop` for application indepentent memory watching and even ran through `fSanitizer` and `HTop` strongly suggests that theres no memory leak at all, infact the application memory stays incredibly stable over a 5min runtime. Looking further into the outpout of `Valgrind` its evident that these suppsoed lost memory dumps are from the `wayland` + `nvidia-open-dkms` stack where this behaviour is actually expected as the driver and compositor leave it up to the OS to clean its memory up afterwards.
	Closing this Bug on 10/03/26  