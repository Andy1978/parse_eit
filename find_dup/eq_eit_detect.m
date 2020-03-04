# Algorithmen zum testen gleicher EITs entwickeln
# Leerzeichen trennt

fid = fopen ("TBBT_german_good.lst", "r");
row = 1;
col = 1;
slist = {};
while (! feof (fid))
  tmp = fgetl (fid);

  if (numel (tmp) == 0)
    if (col > 1)
      row++;
    endif
    col = 1;
  else
    slist{row, col++} = tmp;
  endif

endwhile
fclose (fid);

function [n, w] = eq_words (a, b)
  a = unique (strsplit(a));
  b = unique (strsplit(b));
  w = intersect (a,b);
  n = numel (w) / ((numel(a) + numel(b)) / 2);
endfunction

# Alternative Idee: kurze Wörter entfernen, Längere stärker gewichten
function [n, w] = eq_words2 (a, b)
  a = unique (strsplit(a));
  b = unique (strsplit(b));
   
  # ganz kurze Wörter entfernen
  a(cellfun ("numel", a) < 4) = [];
  b(cellfun ("numel", b) < 4) = [];

  w = intersect (a,b);

  sa = sum (cellfun ("numel", a));
  sb = sum (cellfun ("numel", b));
  sw = sum (cellfun ("numel", w));

  n = sw/((sa+sb)/2);
endfunction 

# Todo: auch _bad einlesen und schauen was besser ist

for k = 1:rows(slist)

  [n] = eq_words (slist{k,:});
  [n2, w2] = eq_words2(slist{k,:});
  
  printf ("equality index %.2f%% %.2f%%\n", n * 1e2, n2 * 1e2);
  #printf ("%s\n\n", slist{k,:})

endfor
