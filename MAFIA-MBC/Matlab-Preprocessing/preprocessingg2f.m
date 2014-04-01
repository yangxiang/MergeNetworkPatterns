FILEBASE='genes_to_phenotype';
FILENAME=[FILEBASE,'.txt'];

fid0 = fopen(FILENAME,'r');
tline=fgetl(fid0);% remove col info line
tline=fgetl(fid0);

geneList=cell(1,1);
AllGenes=[];
phenotypeList=cell(1,1);
AllPhenotypes=[];

i=1;
while ischar(tline)
    [sid eid]=regexpi(tline,'\t');
    genes = tline(1:sid(2)-1);
    phenotype = tline(sid(2)+1:end);

    tmp=textscan(genes,'%s','Delimiter','');
    AllGenes=[AllGenes, tmp{1}'];
    geneList{i}=tmp{1}';
    
    tmp=textscan(phenotype,'%s', 'Delimiter', ';');
    tmpp=cell(1,1);
    AllPhenotypes=[AllPhenotypes, tmp{1}'];
    phenotypeList{i}=tmp{1};

    tline=fgetl(fid0);
    i=i+1;
end
fclose(fid0);
AllPhenotypes=unique(AllPhenotypes);
%AllGenes=unique(AllGenes);

FileGeneList=['..\..\SumNetwork\datasets\',FILEBASE,'_GeneList.txt'];
FilePhenotypeList=['..\..\SumNetwork\datasets\',FILEBASE,'_PhenotypeList.txt'];

fid1=fopen(FileGeneList,'wt');
fid2=fopen(FilePhenotypeList,'wt');

for i=1:length(AllGenes)
    fprintf(fid1,AllGenes{i});
    fprintf(fid1,'\n');
end
fclose(fid1);

for i=1:length(AllPhenotypes)
    fprintf(fid2,AllPhenotypes{i});
    fprintf(fid2,'\n');
end
fclose(fid2);

FileDAT=['..\datasets\',FILEBASE,'.dat'];
fid3=fopen(FileDAT, 'wt');
for i=1:length(geneList)
    %Assertion Begin
    if length(geneList{1})~=1
        fprintf('Assertion fails.');
        break;
    end
    if strcmp(geneList{i}(1),AllGenes{i})~=1
        fprintf('Assertion fails.');
        break;
    end
    if length(AllGenes)~=length(unique(AllGenes));
        fprintf('Assertion fails.');
        break;
    end        
    %Assertion End
    for j=1:length(phenotypeList{i})
        col=strmatch(phenotypeList{i}{j},AllPhenotypes, 'exact');
        fprintf(fid3,'%d ',col);
    end
    fprintf(fid3,'\n');
end







