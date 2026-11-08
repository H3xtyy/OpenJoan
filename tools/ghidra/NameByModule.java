// NameByModule.java — OpenJoan Ghidra post-script.
// Attributes functions to their original source module using the embedded "d:\ww\...cpp" path strings
// (assert/log call sites) and names them <dir>::<file>::fn_<addr>; names the CRT entry and WinMain;
// exports docs/re/functions/_index.md and a JSON name map for re-applying names in other tools (rea).
// Usage (headless): analyzeHeadless <proj_dir> <proj> -import joan.exe -postScript NameByModule.java <index.md> <names.json> [retail|steam]
// The third argument selects the fixed-point table (addresses differ per build); default: none.
//@category OpenJoan
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.regex.*;

public class NameByModule extends GhidraScript {
    // Known fixed points, verified in docs/re/BINARY.md (retail build 236ddfa9…). Applied only for "retail".
    private static final Object[][] FIXED_RETAIL = {
        {0x7b5f32L, "WinMainCRTStartup", "MSVC6 CRT entry (original entry point); the .ekvam stub transfers here. RE: docs/re/BINARY.md"},
        {0x56bb30L, "WinMain", "Enlight WinMain: CPU check (vertexf_check_cpu), CONFIG.DAT, CD presence check, game init. RE: docs/re/BINARY.md"},
        {0x88eb83L, "stub_entry", "Pre-entry stub in .ekvam (resolves CreateThread/OpenEventA/Reg*/PostMessageA by name). RE: docs/re/BINARY.md"},
    };
    // Steam build (3ef30477…): filled in from docs/re/BINARY.md §Steam build.
    private static final Object[][] FIXED_STEAM = {
        {0x7b71e6L, "WinMainCRTStartup", "MSVC7 CRT entry = PE entry point. RE: docs/re/BINARY.md"},
        {0x420ea0L, "WinMain", "Enlight WinMain: v3d swap check, vertexf_check_cpu, CONFIG.DAT, GlobalMemoryStatus, game init (no CD check). RE: docs/re/BINARY.md"},
        {0x447150L, "v3d_swap_check", "Replaces V3D_87.DLL with the CPU-optimised variant and asks for a restart (same logic as JoanofArc.exe). RE: docs/re/BINARY.md"},
    };

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        Path indexMd = Paths.get(args.length > 0 ? args[0] : "functions_index.md");
        Path namesJson = Paths.get(args.length > 1 ? args[1] : "names.json");
        String build = args.length > 2 ? args[2] : "";
        Object[][] FIXED = build.equals("retail") ? FIXED_RETAIL : build.equals("steam") ? FIXED_STEAM : new Object[0][];
        Listing listing = currentProgram.getListing();
        SymbolTable st = currentProgram.getSymbolTable();
        ReferenceManager rm = currentProgram.getReferenceManager();
        Pattern srcRe = Pattern.compile("^[A-Za-z]:\\\\ww\\\\(.+)$");

        // 1. collect: function -> {path -> ref count}
        Map<Function, Map<String, Integer>> votes = new TreeMap<>(Comparator.comparing(f -> f.getEntryPoint()));
        int strings = 0;
        for (Data d : listing.getDefinedData(true)) {
            Object v = d.getValue();
            if (!(v instanceof String)) continue;
            Matcher m = srcRe.matcher((String) v);
            if (!m.matches()) continue;
            strings++;
            String rel = m.group(1);
            for (Reference r : rm.getReferencesTo(d.getAddress())) {
                Function f = listing.getFunctionContaining(r.getFromAddress());
                if (f == null) continue;
                votes.computeIfAbsent(f, k -> new HashMap<>()).merge(rel, 1, Integer::sum);
            }
        }
        println("source-path strings: " + strings + ", functions referencing them: " + votes.size());

        // 2. name
        Map<String, String> names = new TreeMap<>();
        List<String[]> rows = new ArrayList<>();
        int renamed = 0;
        for (Map.Entry<Function, Map<String, Integer>> e : votes.entrySet()) {
            Function f = e.getKey();
            String best = e.getValue().entrySet().stream().max(Map.Entry.comparingByValue()).get().getKey();
            String[] parts = best.replace(".cpp", "").replace(".h", "").split("\\\\");
            Namespace ns = currentProgram.getGlobalNamespace();
            for (String p : parts) ns = st.getOrCreateNameSpace(ns, sanitize(p), SourceType.USER_DEFINED);
            String name = "fn_" + String.format("%08x", f.getEntryPoint().getOffset());
            if (f.getSymbol().getSource() == SourceType.DEFAULT || f.getName().startsWith("FUN_") || f.getName().startsWith("fn_")) {
                f.getSymbol().setNameAndNamespace(name, ns, SourceType.USER_DEFINED);
                renamed++;
            }
            String conflicts = e.getValue().size() > 1 ? " (also refs: " + e.getValue().keySet() + ")" : "";
            f.setComment("OpenJoan module attribution: " + best + conflicts);
            createBookmark(f.getEntryPoint(), "OpenJoan/module", best);
            names.put(String.format("0x%08x", f.getEntryPoint().getOffset()), f.getName(true));
            rows.add(new String[]{String.format("0x%08x", f.getEntryPoint().getOffset()), f.getName(true), best, String.valueOf(f.getBody().getNumAddresses()), conflicts.isEmpty() ? "" : "multi-file refs"});
        }
        for (Object[] fx : FIXED) {
            Address a = toAddr((Long) fx[0]);
            Function f = listing.getFunctionAt(a);
            if (f == null) { println("fixed point missing function at " + a); continue; }
            f.setName((String) fx[1], SourceType.USER_DEFINED);
            f.setComment((String) fx[2]);
            names.put(String.format("0x%08x", a.getOffset()), (String) fx[1]);
            rows.add(new String[]{String.format("0x%08x", a.getOffset()), (String) fx[1], "(fixed point)", String.valueOf(f.getBody().getNumAddresses()), ""});
        }
        println("renamed " + renamed + " functions");

        // 3. export
        long total = listing.getFunctions(true).hasNext() ? currentProgram.getFunctionManager().getFunctionCount() : 0;
        StringBuilder sb = new StringBuilder();
        sb.append("# " + currentProgram.getName() + " — function index (auto-generated by tools/ghidra/NameByModule.java)\n\n");
        sb.append("Binary: `" + currentProgram.getName() + "` (" + (build.isEmpty() ? "unspecified build" : build + " build") + "), image base 0x400000. Total functions found by Ghidra: ")
          .append(total).append(". Functions attributed to a source module via embedded path strings: ")
          .append(votes.size()).append(" (").append(strings).append(" path strings).\n\n")
          .append("Naming: `<dir>::<file>::fn_<addr>` from `d:\\ww\\<dir>\\<file>.cpp`. A function voting for several files keeps the\n")
          .append("most-referenced one and is flagged `multi-file refs`. Regenerate: see docs/re/BINARY.md §Ghidra.\n\n");
        sb.append("| address | name | source file | size (bytes) | note |\n|---|---|---|---|---|\n");
        rows.sort(Comparator.comparing(r -> r[0]));
        for (String[] r : rows) sb.append("| `").append(r[0]).append("` | `").append(r[1]).append("` | `").append(r[2]).append("` | ").append(r[3]).append(" | ").append(r[4]).append(" |\n");
        Files.createDirectories(indexMd.toAbsolutePath().getParent());
        Files.write(indexMd, sb.toString().getBytes(StandardCharsets.UTF_8));
        StringBuilder js = new StringBuilder("{\n");
        boolean first = true;
        for (Map.Entry<String, String> n : names.entrySet()) {
            if (!first) js.append(",\n");
            first = false;
            js.append("  \"").append(n.getKey()).append("\": \"").append(n.getValue().replace("\\", "\\\\").replace("\"", "\\\"")).append("\"");
        }
        js.append("\n}\n");
        Files.write(namesJson, js.toString().getBytes(StandardCharsets.UTF_8));
        println("wrote " + indexMd + " and " + namesJson);
    }

    private static String sanitize(String s) {
        return s.replaceAll("[^A-Za-z0-9_]", "_");
    }
}
