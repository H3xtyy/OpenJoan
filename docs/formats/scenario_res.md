# `scenarioN.res` — packed text-level bundle (`map/scenarioN/scenarioN.res`)

Binary container holding the text sources of one level. Sample `map/scenario1/scenario1.res` (411,055 B), verified
against the file. Reader not identified (the level loader 0x4F2F80 pulls the text data via `ilib::reslx`
0x59BD60/0x59BE20/0x59BF70 and `filetxt` 0x589780, not analysed). The loose files `S1L1.pla/.stg/.uni` also exist
next to it, so the bundle is probably an optional pre-packed variant. Status: layout `verified`, consumer `inferred`.

| offset | size | field |
|---|---|---|
| 0x00 | u32 | `count` = 3 |
| 0x04 + i*38 | char[34] `name`, u32 `offset` | `S1L1_PLA`, `S1L1_STG`, `S1L1_UNI` |
| 0x04 + count*38 | char[34] (zero) + u32 | sentinel entry: empty name, `offset` = file size |
| 0x9C | payload bytes | text payloads, back to back |

Entry 0: name at 0x04, offset field at 0x26 = 0x9C (payload `CreatePlace` text at 0x9C). Entry 1: name at 0x2A,
offset at 0x4C = 0x1F00 (S1L1_STG). Entry 2: name at 0x50, offset at 0x72 = 0x5E360 (S1L1_UNI). Sentinel at 0x76..0x9B:
offset at 0x98 = 0x645AF = 411,055 (file size). Payload size of entry i = next offset - offset. So `S1L1_PLA` = 0x9C..0x1F00
(`CreatePlace` blocks, see block grammar), `S1L1_STG` = 0x1F00..0x5E360 (~378 KB), `S1L1_UNI` = 0x5E360..0x645AF
(`CreateUnit` blocks). Name suffix = extension of the original text file. Payload text uses the same grammar as the loose files
([block.md](block.md), [uni.md](uni.md)).

## Verified on all seven bundles
`map/scenarioN/scenarioN.res` (N = 1..7) all parse as ResLX ([res.md](res.md) census). Entry names are `<level>_PLA|STG|UNI`; scenario 4 additionally has
numbered sub-level sets `S4L1_1_*` .. `S4L1_4_*` and its `S4L1_PLA` / `S4L1_UNI` payloads are empty (0 bytes). Payloads are all text.
