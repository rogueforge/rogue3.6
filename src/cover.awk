# Summarise 'gcov -f' output: per file, then the functions never called.
/^Function /  { name = $2; gsub(/'/, "", name); kind = "f"; next }
/^File /      { name = $2; gsub(/'/, "", name); kind = "F"; next }
/^Lines executed:/ {
    split($2, a, ":"); pct = a[2] + 0; n = $4
    if (kind == "F") { files[name] = pct; flines[name] = n; tot += n; hit += n * pct / 100 }
    else if (kind == "f") { nfunc++; if (pct == 0) { never[++nn] = name; nlines[nn] = n } }
    kind = ""
}
END {
    printf "%-14s %6s %7s\n", "file", "lines", "run"
    for (f in files) printf "%-14s %6d %6.1f%%\n", f, flines[f], files[f] | "sort -k3 -n"
    close("sort -k3 -n")
    printf "\n%d of %d lines run (%.1f%%)\n", hit, tot, 100 * hit / tot
    printf "%d of %d functions never called:\n", nn, nfunc
    for (i = 1; i <= nn; i++) printf "  %s (%d lines)\n", never[i], nlines[i]
}
