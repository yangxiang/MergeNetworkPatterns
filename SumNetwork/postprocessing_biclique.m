clear;
close all;
Genelist_Length_Threshold=5;
Phenolist_Length_Threshold=1;

inputfilepath='.\datasets\';
file_row='genes_to_phenotype_GeneList.txt';
file_col='genes_to_phenotype_PhenotypeList.txt';
fid_row=fopen(strcat(inputfilepath, file_row),'r');
fid_col=fopen(strcat(inputfilepath, file_col));
rownamelist=textscan(fid_row, '%s');
colnamelist=textscan(fid_col, '%s', 'delimiter', '\n');
fclose(fid_row);
fclose(fid_col);

outputfilepath='.\output\';
file_patternid='g2p_S_07_2nd.output';
fid_pattern=fopen(strcat(outputfilepath, file_patternid));
line=fgetl(fid_pattern);
patterns_rowinfo=[];
patterns_colinfo=[];
patterns_densinfo=[];
while ischar(line);
    [sid eid]=regexpi(line, ';');% Assume there are two semicolons each line
    line_left=line(1:sid(1)-1);
    line_right=line(sid(1)+1:sid(2)-1);
    line_dens=line(sid(2)+1:end);
    row_numbers=textscan(line_left, '%d');
    col_numbers=textscan(line_right,'%d');
    patterns_rowinfo=[patterns_rowinfo, row_numbers];
    patterns_colinfo=[patterns_colinfo, col_numbers];
    patterns_densinfo=[patterns_densinfo, {line_dens}];
    line=fgetl(fid_pattern);
end
fclose(fid_pattern);

file_decoded=['g2p_S_07_2nd.decoded_g', int2str(Genelist_Length_Threshold), '_output'];
fid_decoded=fopen(strcat(outputfilepath, file_decoded), 'wt');
for k=1:length(patterns_rowinfo)
    if length(patterns_rowinfo{k})<Genelist_Length_Threshold
        continue;
    end
    if length(patterns_colinfo{k})<Phenolist_Length_Threshold
        continue;
    end   
    for i=1:length(patterns_rowinfo{k})
        fprintf(fid_decoded, '%s ', rownamelist{1}{patterns_rowinfo{k}(i)});
    end
    fprintf(fid_decoded, '; ');
    for j=1:length(patterns_colinfo{k})
        fprintf(fid_decoded, '%s ', colnamelist{1}{patterns_colinfo{k}(j)});
    end
    fprintf(fid_decoded, '; ');
    fprintf(fid_decoded, '%s\n', patterns_densinfo{k});
end
fclose(fid_decoded);