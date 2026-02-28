
- [ ] Remove `gwinmasks` and use `Renderer::clear`, `Renderer::clearColor` instead 🔽 ➕ 2026-02-27 
- [ ] Implement `Renderer` being able to use multiple texture units/slots and eficiently sort between them. ⏫ ➕ 2026-02-27 
- [ ] Implement a `Renderer` based Text fn 🔼 ➕ 2026-02-27 
- [ ] Implement delayed destruction of `Renderer` managed objects.
	Can be done by creating an `EngineState::destroyObj` that pushes it to a vector, then in `windowContext::endFrame` we iterate through the vec and explicitly call delete on the obj
	Might need to be templated to so above.










- [ ] This is a debug message #debug 
- [ ] Another debug message ➕ 2026-01-01 #debug