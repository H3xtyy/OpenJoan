// ExportCallTree.java — OpenJoan Ghidra script: decompile the given entry functions and their callees to a
// bounded depth (default 2), one family per output directory, with an INDEX.md (function → callees → strings).
// CRT/import thunks (address ≥ CRT_START) and external functions are listed but not decompiled.
// Usage: analyzeHeadless <proj> <name> -process joan_steam.exe -noanalysis -readOnly
//        -postScript ExportCallTree.java <outdir> <families-file> [depth]
// <families-file>: lines "family 0xaddr 0xaddr 0xlo-0xhi …" (# comments allowed; a range exports every function in it).
//@category OpenJoan
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.symbol.*;
import ghidra.program.model.address.*;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.regex.*;

public class ExportCallTree extends GhidraScript {
    private static final long CRT_START = 0x7b0000L;

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        Path out = Paths.get(args[0]);
        int maxDepth = args.length > 2 ? Integer.parseInt(args[2]) : 2;
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        Pattern strRe = Pattern.compile("\"((?:[^\"\\\\]|\\\\.){2,40})\"");
        for (String line : Files.readAllLines(Paths.get(args[1]), StandardCharsets.UTF_8)) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) continue;
            String[] parts = line.split("\\s+");
            String family = parts[0];
            Path dir = out.resolve(family);
            Files.createDirectories(dir);
            Map<Function, Integer> depthOf = new LinkedHashMap<>();
            Deque<Function> queue = new ArrayDeque<>();
            for (int i = 1; i < parts.length; i++) {
                // "0xlo-0xhi": every function whose entry lies in [lo, hi) becomes a root (whole-module export).
                // "^0xaddr": every function that calls 0xaddr, directly or through a thunk, becomes a root.
                if (parts[i].startsWith("^")) {
                    Address target = toAddr(Long.decode(parts[i].substring(1)));
                    Set<Address> entries = new HashSet<>();
                    entries.add(target);
                    for (Function t : currentProgram.getFunctionManager().getFunctions(true))
                        if (t.isThunk() && t.getThunkedFunction(true) != null && t.getThunkedFunction(true).getEntryPoint().equals(target))
                            entries.add(t.getEntryPoint());
                    for (Address e : entries)
                        for (Reference r : getReferencesTo(e)) {
                            Function c = getFunctionContaining(r.getFromAddress());
                            if (c != null && !c.isThunk() && !depthOf.containsKey(c)) { depthOf.put(c, 0); queue.add(c); }
                        }
                    continue;
                }
                int dash = parts[i].indexOf('-', 1);
                if (dash > 0) {
                    long lo = Long.decode(parts[i].substring(0, dash)), hi = Long.decode(parts[i].substring(dash + 1));
                    for (Function f : currentProgram.getFunctionManager().getFunctions(toAddr(lo), true)) {
                        if (f.getEntryPoint().getOffset() >= hi) break;
                        if (!depthOf.containsKey(f)) { depthOf.put(f, 0); queue.add(f); }
                    }
                    continue;
                }
                Function f = getFunctionAt(toAddr(Long.decode(parts[i])));
                if (f == null) f = getFunctionContaining(toAddr(Long.decode(parts[i])));
                // A thunk root (e.g. a vtable slot pointing into the jump table) is replaced by its target.
                if (f != null && f.isThunk() && f.getThunkedFunction(true) != null) f = f.getThunkedFunction(true);
                if (f != null && !depthOf.containsKey(f)) { depthOf.put(f, 0); queue.add(f); }
            }
            StringBuilder index = new StringBuilder("| depth | address | name | size | callees | strings |\n|---|---|---|---|---|---|\n");
            while (!queue.isEmpty()) {
                Function f = queue.poll();
                int d = depthOf.get(f);
                Function real = f.isThunk() ? f.getThunkedFunction(true) : f;
                if (real == null || real.isExternal() || real.getEntryPoint().getOffset() >= CRT_START) {
                    index.append("| ").append(d).append(" | `").append(f.getEntryPoint()).append("` | ").append(f.getName(true)).append(" | - | (external/CRT, not exported) | |\n");
                    continue;
                }
                DecompileResults res = dec.decompileFunction(real, 60, monitor);
                String code = res.decompileCompleted() ? res.getDecompiledFunction().getC() : "// decompile failed: " + res.getErrorMessage();
                String fname = String.format("%08x_%s.c", real.getEntryPoint().getOffset(), real.getName(true).replace("::", "__").replaceAll("[^A-Za-z0-9_.]", "_"));
                Files.write(dir.resolve(fname), ("// " + real.getName(true) + "  depth=" + d + "\n" + code).getBytes(StandardCharsets.UTF_8));
                List<String> callees = new ArrayList<>();
                for (Function c : real.getCalledFunctions(monitor)) {
                    Function cr = c.isThunk() ? c.getThunkedFunction(true) : c;
                    if (cr == null) cr = c;
                    callees.add(String.format("0x%08x", cr.getEntryPoint().getOffset()));
                    if (d < maxDepth && !depthOf.containsKey(cr) && !cr.isExternal() && cr.getEntryPoint().getOffset() < CRT_START) {
                        depthOf.put(cr, d + 1); queue.add(cr);
                    }
                }
                Set<String> strs = new TreeSet<>();
                Matcher m = strRe.matcher(code);
                while (m.find()) strs.add(m.group(1));
                index.append("| ").append(d).append(" | `").append(String.format("0x%08x", real.getEntryPoint().getOffset())).append("` | ")
                     .append(real.getName(true)).append(" | ").append(real.getBody().getNumAddresses()).append(" | ")
                     .append(String.join(" ", callees)).append(" | ").append(String.join(" \\| ", strs).replace("|", "\\|")).append(" |\n");
            }
            Files.write(dir.resolve("INDEX.md"), ("# " + family + " — call tree export (depth ≤ " + maxDepth + ")\n\n" + index).getBytes(StandardCharsets.UTF_8));
            println(family + ": " + depthOf.size() + " functions");
        }
        dec.dispose();
    }
}
