// ExportByStrings.java — OpenJoan Ghidra script: for each literal in a list, find the string's references,
// decompile every function that references it to <outdir>/<addr>_<name>.c and write <outdir>/INDEX.md
// (literal → functions). Used to export the loaders of a format family for documentation agents
// (CLAUDE.md §5a: export first, then fan out).
// Usage: analyzeHeadless <proj> <name> -process joan_steam.exe -noanalysis -postScript ExportByStrings.java <outdir> <literals-file>
// <literals-file>: one literal per line (UTF-8, as it appears in the binary; cp1252 bytes are matched by char value).
//@category OpenJoan
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.symbol.Reference;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;

public class ExportByStrings extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        Path out = Paths.get(args[0]);
        Files.createDirectories(out);
        List<String> literals = Files.readAllLines(Paths.get(args[1]), StandardCharsets.UTF_8);
        Memory mem = currentProgram.getMemory();
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        StringBuilder index = new StringBuilder("| literal | string address | referencing functions |\n|---|---|---|\n");
        Set<Function> done = new HashSet<>();
        for (String lit : literals) {
            lit = lit.trim();
            if (lit.isEmpty() || lit.startsWith("#")) continue;
            byte[] needle = lit.getBytes(StandardCharsets.ISO_8859_1);
            Address a = mem.findBytes(currentProgram.getMinAddress(), needle, null, true, monitor);
            List<String> hits = new ArrayList<>();
            List<String> addrs = new ArrayList<>();
            int guard = 0;
            while (a != null && guard++ < 50) {
                // only C-string starts (previous byte NUL) to skip substrings
                boolean start = true;
                try { start = mem.getByte(a.subtract(1)) == 0; } catch (Exception e) {}
                if (start) {
                    addrs.add(String.format("0x%08x", a.getOffset()));
                    for (Reference r : getReferencesTo(a)) {
                        Function f = getFunctionContaining(r.getFromAddress());
                        if (f == null) continue;
                        String tag = String.format("`0x%08x` %s", f.getEntryPoint().getOffset(), f.getName(true));
                        if (!hits.contains(tag)) hits.add(tag);
                        if (done.add(f)) {
                            DecompileResults res = dec.decompileFunction(f, 60, monitor);
                            String code = res.decompileCompleted() ? res.getDecompiledFunction().getC() : "// decompile failed: " + res.getErrorMessage();
                            String fname = String.format("%08x_%s.c", f.getEntryPoint().getOffset(), f.getName(true).replace("::", "__").replaceAll("[^A-Za-z0-9_.]", "_"));
                            Files.write(out.resolve(fname), ("// " + f.getName(true) + "  (referenced literal: " + lit + ")\n" + code).getBytes(StandardCharsets.UTF_8));
                        }
                    }
                }
                a = mem.findBytes(a.add(1), needle, null, true, monitor);
            }
            index.append("| `").append(lit.replace("|", "\\|")).append("` | ").append(String.join(", ", addrs))
                 .append(" | ").append(String.join("; ", hits)).append(" |\n");
        }
        dec.dispose();
        Files.write(out.resolve("INDEX.md"), index.toString().getBytes(StandardCharsets.UTF_8));
        println("exported " + done.size() + " functions to " + out);
    }
}
