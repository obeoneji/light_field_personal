im = im2double(imread('light_field_slabs.bmp'));

[m,n,ch] = size(im);

face_num = 4;
ver_im_num = 1;
hor_im_num = 8;
sub_row = m/face_num/ver_im_num;
sub_col = n/hor_im_num;

for i = 1:face_num
    for j = 1:ver_im_num
        for k = 1:hor_im_num
            st_row = (i-1)*ver_im_num*sub_row+(j-1)*sub_row + 1;
            end_row = st_row + sub_row-1;
            st_col = (k-1)*sub_col + 1;
            end_col = st_col + sub_col - 1;
            im_out(i,j,k).im = im(st_row:end_row,st_col:end_col,:);
            imwrite(im_out(i,j,k).im,['output\' num2str(i) '_' num2str(j) '_' num2str(k) '_' '.bmp'])
        end
    end
end