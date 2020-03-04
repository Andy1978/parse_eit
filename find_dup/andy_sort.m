# Die durch parse_eit (cropped aka foo) erzeugte Liste list.csv abarbeiten

res_fn = "result.log";
if (exist (res_fn, "file"))
  load (res_fn);
else
  if (! exist ("long", "var"))
    fid = fopen ("list.csv", "r");
    cnt = 0;
    long={};
    while (! feof (fid))
      s = fgetl (fid);
      c = strsplit (s, ";", "collapsedelimiters", false);
      long{++cnt} = strjoin (c(4:end), "");
      fn{cnt} = c{1};
    endwhile
    fclose (fid);
    clear -x fn long res_fn
  endif

  # Satzzeichen ,. raus
  long = cellfun (@(x) regexprep (x, "[,.]", ""), long, "UniformOutput", false);

  function [n, w] = eq_words (a, b)
    a = unique (strsplit(a));
    b = unique (strsplit(b));
    w = intersect (a,b);
    n = numel (w) / ((numel(a) + numel(b)) / 2);
  endfunction

  C = nchoosek (1:numel(long), 2);

  res = [];
  cnt = 0;
  for k=1:rows(C)
    idx = C(k, :);
    n = eq_words (long{idx(1)}, long{idx(2)});
    res(++cnt, :) = [idx(1), idx(2), n];

    if (! mod (cnt, 200))
      printf ("%i/%i\n", cnt, rows(C))
    endif

    if (n > 0.5)
      printf ("%i %i %.2f\n", idx(1), idx(2), n);
      #long{idx(1)}
      #long{idx(2)}
    endif
  endfor

  save (res_fn, "fn", "long", "res");
endif

# Nach "match index" sortieren...
[~, idx] = sort (res(:,3), "descend");
res_s = res(idx, :);

# Wenn es z.B. drei gleiche EITs gibt, besteht die Gefahr alle Dateien zu löschen
for k=1:rows(res_s)
  
  tmp = res_s(k, :);

  if (tmp(3) > 0.35)
    idx_a = tmp(1);
    idx_b = tmp(2);

    ts_fn_a = strrep(fn{idx_a}, ".eit", ".ts");
    ts_fn_b = strrep(fn{idx_b}, ".eit", ".ts");

    # Nur wenn beide noch existieren, die größere Datei löschen
    if (   exist (ts_fn_a, "file")
        && exist (ts_fn_b, "file"))

        size_a = stat (ts_fn_a).size / 1024^2;
        size_b = stat (ts_fn_b).size / 1024^2;

        printf ("equality index %.1f%%\n", tmp(3) * 1e2);
        printf ("%3i %s %.2fMiB %s\n", idx_a, ts_fn_a, size_a, long{idx_a});
        printf ("%3i %s %.2fMiB %s\n", idx_b, ts_fn_b, size_b, long{idx_b});

        r = input ("Größere Datei löschen? Y/n ", "s");
        
        if (!numel(r) || r=="Y")
          printf ("löschen\n")
          if (size_a > size_b)
            unlink (ts_fn_a);
            unlink (fn{idx_a});
          else
            unlink (ts_fn_b);
            unlink (fn{idx_b});
          endif
        endif
        printf ("\n");

    endif
  endif

endfor

