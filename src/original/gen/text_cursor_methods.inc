// Generated (re/facades/gen2.py): typed members for the TextCursor functions game code calls through their slots.
// Each forwards to the function's slot (the registered replacement).

/// 0x0058b9c0 TextCursor::TextCursor
void* construct(const char* a0);
/// 0x0058ba70 TextCursor::get
std::int32_t get();
/// 0x0058bbc0 TextCursor::next
void* next();
/// 0x0058bbf0 TextCursor::postinc
void* postinc(void* a0, std::int32_t a1);
/// 0x0058bd30 TextCursor::put
void* put(void* a0, std::uint32_t a1);
/// 0x0058c120 TextCursor::advanced
void* advanced(void* a0, std::int32_t a1);
/// 0x0058c190 TextCursor::length
std::int32_t length();
/// 0x0058c2e0 TextCursor::call_a157e8
std::uint32_t call_a157e8();
/// 0x0058ceb0 TextCursor::find
void* find(void* a0, std::uint32_t a1);
/// 0x0058b990 Ptr::Ptr
void* construct_null();
/// 0x0058cd40 TextBuffer::end_of
template <class T1>
void* copy_end(void* a0, T1 a1) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); return ::joan::call::thiscall<void*>(::joan::fn::TextBuffer_end_of, this, a0, a1); }
/// 0x0058c400 TextCursor::call_a157ec
template <class T0>
std::uint32_t copy_dispatch(T0 a0) { static_assert(!std::is_pointer_v<T0> && std::is_trivially_copyable_v<T0>, "a struct by value"); return ::joan::call::thiscall<std::uint32_t>(::joan::fn::TextCursor_call_a157ec, this, a0); }
/// 0x0058cdb0 TextBuffer::assign
template <class T1, class T2>
void* assign_range(void* a0, T1 a1, T2 a2) { static_assert(!std::is_pointer_v<T1> && std::is_trivially_copyable_v<T1>, "a struct by value"); static_assert(!std::is_pointer_v<T2> && std::is_trivially_copyable_v<T2>, "a struct by value"); return ::joan::call::thiscall<void*>(::joan::fn::TextBuffer_assign, this, a0, a1, a2); }
