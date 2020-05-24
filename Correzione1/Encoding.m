% Email: enrico.leka@studenti.unipd.it

clear;        
fileDaLeggere=fopen('input.data', 'r');            % Apertura del file in lettura contenente i dati in UCS4
fileInCuiScrivere=fopen('UTF8.data', 'w');            % Apertura del file in scrittora, dove scriverò i dati in UTF-8
while ~feof(fileDaLeggere)            % Itero fino a che non arrivo all'End of File 
    A = fread(fileDaLeggere,4);       % Leggo dal file 4 byte, cioè 32 bit della rappresentazione UCS4 e li salvo in A, (A sarà [X, X, X, X])
    if isempty(A)               % Se A è vuoto, mi fermo 
        break
    end               
    matriceBinaria = dec2bin(A, 8);       % Converto il contenuto di A da decimale a binario di 8 bit (4 --> 00000100)       
    binario = strcat(matriceBinaria(4, :), matriceBinaria(3, :), matriceBinaria(2, :), matriceBinaria(1, :));        % Concateno i 4 byte nell'ordine corretto (mettendo i più significativi a SX) 
    zeriIniziali = 0;                  
    i = 1;
    while (i <= 32 && binario(i) ~= '1')            % Calcolo il numero di zeri iniziali nei 32 bit
        i = i + 1;
        zeriIniziali = zeriIniziali + 1;         
    end                                      
    nBitInformazione = 32 - zeriIniziali;   % Calcolo il numero di bit di informazione utile, togliendo gli zeri iniziali che non mi servono. In base a quanti bit       
                                            % di informazione calcolo, avrò un numero diverso di byte nella codifica UTF-8. Da qui in poi la serie di if sono 
                                            % una serie di controlli per ricondurmi al caso speficico
    if (nBitInformazione <= 7)
        %BYTE 1: 0xxxxxxx
        stringaOutput = strcat("0", extractBetween(binario, 26, 32));       % Quindi ad uno 0, aggiungo gli ultimi 7 bit    
    elseif (nBitInformazione > 7 && nBitInformazione <= 11)
        %BYTE 1: 110xxxxx BYTE 2: 10xxxxxx              % Nel BYTE 1 andranno 110 + 5 bit presi dal 22 al 26 esimo, nel BYTE 2 andranno 10 + 5 bit presi dal 27 al 32 esimo 
        stringaOutput = strcat("110", extractBetween(binario, 22, 26), "10", extractBetween(binario, 27, 32));  
    elseif (nBitInformazione > 11 && nBitInformazione <= 16)
        %BYTE 1: 1110xxxx BYTE 2: 10xxxxxx BYTE 3: 10xxxxxx             % Stesso ragionamento di prima con il BYTE 3 aggiunto
        stringaOutput = strcat("1110", extractBetween(binario, 17, 20), "10", extractBetween(binario, 21, 26), "10", extractBetween(binario, 27, 32));  
    elseif (nBitInformazione > 16 && nBitInformazione <= 21)
        %BYTE 1: 11110xxx BYTE 2: 10xxxxxx BYTE 3: 10xxxxxx BYTE 4: 10xxxxxx            % Stesso ragionamento di prima con il BYTE 4 aggiunto
        stringaOutput = strcat("11110", extractBetween(binario, 12, 14), "10", extractBetween(binario, 15, 20), "10", extractBetween(binario, 21, 26), "10", extractBetween(binario, 27, 32));    
    elseif (nBitInformazione > 21 && nBitInformazione <= 26)
        %BYTE 1: 111110xx BYTE 2: 10xxxxxx BYTE 3: 10xxxxxx BYTE 4: 10xxxxxx BYTE 5: 10xxxxxx               % Stesso ragionamento di prima con il BYTE 5 aggiunto
        stringaOutput = strcat("111110", extractBetween(binario, 7, 8), "10", extractBetween(binario, 9, 14), "10", extractBetween(binario, 15, 20), "10" ,extractBetween(binario, 21, 26), "10", extractBetween(binario, 27, 32));     
    elseif (nBitInformazione > 26 && nBitInformazione <= 31)
        %BYTE 1: 1111110x BYTE 2: 10xxxxxx BYTE 3: 10xxxxxx BYTE 4: 10xxxxxx BYTE 5: 10xxxxxx BYTE 6:10xxxxxx                   % Stesso ragionamento di prima con il BYTE 6 aggiunto
        stringaOutput = strcat("1111110", extractBetween(binario, 2, 2), "10", extractBetween(binario, 3, 8), "10", extractBetween(binario, 9, 14), "10", extractBetween(binario, 15, 20), "10", extractBetween(binario, 21, 26), "10", extractBetween(binario, 27, 32));     
    elseif (nBitInformazione>31)        % Caso dei 32 bit 
        %BYTE 1: 11111110 BYTE 2: 100000xx BYTE 3: 10xxxxxx BYTE 4: 10xxxxxx BYTE 5: 10xxxxxx BYTE 6:10xxxxxx BYTE 7:10xxxxxx                   
        stringaOutput = strcat("11111110", "100000", extractBetween(binario, 1, 2), "10", extractBetween(binario, 3, 8), "10", extractBetween(binario, 9, 14), "10", extractBetween(binario, 15, 20), "10", extractBetween(binario, 21, 26), "10", extractBetween(binario, 27, 32));   
    end                     
    quantitaByte = strlength(stringaOutput)/8;     % Faccio il calcolo di quanti byte avrò nella codifica UFT-8
    k = 0;  
    while (k < quantitaByte)
        byte = extractBetween(stringaOutput,(k*8)+1,(k*8)+8);         % Estraggo 8 bit alla volta, la prima iterazione da 1 a 8, la seconda iterazione da 9 a 16
        k = k + 1;
        daStampare = bin2dec(byte);              % Faccio la conversione dei bit da binario a decimale 
        fwrite(fileInCuiScrivere, daStampare);          % Scrivo il valore ottenuto sul file  
    end
end
% Chiudo i file 
fclose(fileDaLeggere);            
fclose(fileInCuiScrivere);

% COMMENTI
% -: Il file ha il nome sbagliato
% -: variabile A non è auto-esplicativa, non si capisce immediatamente cosa rappresenti
% -: Nel calcolo del numero di zeri iniziali vengono usate due variabili per fare lo stesso lavoro (i e zeriIniziali) - 15
% +: Ben commentato, sebbene non tutto il codice sia autoesplicativo quando ci sono difficoltà vengono in aiuto i commenti che sono ben scritti.
% -: Al posto dell'ultimo while si sarebbe potuto usare un for - 46
% ~: Alcuni commenti di troppo, lo sanno tutti che fclose serve a chiudere i file, non serve specificarlo - 53

% Voto: 8/10 si poteva fare con un ciclo al posto di un enorme switch-case, però sembra codice sufficientemente manutenibile grazie ai commenti e fa il suo dovere