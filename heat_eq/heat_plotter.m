filename1 = "room_hour_0.csv";
filename2 = "room_hour_10.csv";
at0hours = readmatrix(filename1);
at10hours = readmatrix(filename2);

heatmap(at0hours, 'Colormap', hot);
figure;
heatmap(at10hours, 'Colormap', hot);