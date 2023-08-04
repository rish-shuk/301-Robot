function [random_position] = get_random_location( rows, cols, map)
isNotValid = true;

while (isNotValid)
    min_rows = 1;
    max_rows = max(rows);
    
    min_cols = 1;
    max_cols = max(cols);
    
    
    random_rows = round((max_rows - min_rows).*rand(1000,1) + min_rows);
    random_cols = round((max_cols - min_cols).*rand(1000,1) + min_cols);

    random_row = random_rows(1);
    random_col = random_cols(1);
    
    if map(random_row, random_col) == 0
        isNotValid = false;
    end
end
random_position = [random_row, random_col];

end

