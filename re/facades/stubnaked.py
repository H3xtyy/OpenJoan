import sys,re
s=open(sys.argv[1]).read(); out=[]; pos=0
for m in re.finditer(r'__declspec\(naked\)[^{;]*\{',s):
    if m.start()<pos: continue
    j=m.end()-1; d=0; k=j
    while True:
        c=s[k]; d+=(c=='{')-(c=='}'); k+=1
        if d==0: break
    out.append(s[pos:j]+'{ }'+'\n'*s[j:k].count('\n')); pos=k
out.append(s[pos:]); open(sys.argv[2],'w').write(''.join(out))
