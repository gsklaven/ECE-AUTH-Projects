% Define the constellation points for 16-PAM, 16-PSK, and 16-QAM
M = 16;
constellation_pam = pammod(0:(M-1), M, 0, 'gray');  % 16-PAM
constellation_psk = pskmod(0:(M-1), M, 0, 'gray');  % 16-PSK
constellation_qam = qammod(0:(M-1), M, 'gray');  % 16-QAM

% Normalize constellations so that average symbol energy Es = 1
constellation_pam = normalize_energy(constellation_pam);
constellation_psk = normalize_energy(constellation_psk);
constellation_qam = normalize_energy(constellation_qam);

% Generate 16-CQAM constellation
N = 4; % Change N to other values as needed
constellation_cqam = generate_16CQAM(M, N);
constellation_cqam = normalize_energy(constellation_cqam);

% Calculate Euclidean distances for each modulation
dmin_pam = calculate_dmin(constellation_pam);
dmin_psk = calculate_dmin(constellation_psk);  % Calculate dmin for 16-PSK
dmin_qam = calculate_dmin(constellation_qam);
dmin_cqam = calculate_dmin(constellation_cqam);

% Calculate PAPR for each modulation
calculatePAPR = @(signal) max(abs(signal).^2) / mean(abs(signal).^2);
papr_pam = calculatePAPR(constellation_pam);
papr_psk = calculatePAPR(constellation_psk);
papr_qam = calculatePAPR(constellation_qam);
papr_cqam = calculatePAPR(constellation_cqam);

% Plot PAPR vs dmin
figure;
hold on;
scatter(dmin_pam, papr_pam, 'filled', 'DisplayName', '16-PAM');
scatter(dmin_psk, papr_psk, 'filled', 'DisplayName', '16-PSK');
scatter(dmin_qam, papr_qam, 'filled', 'DisplayName', '16-QAM');
scatter(dmin_cqam, papr_cqam, 'filled', 'DisplayName', '16-CQAM');
xlabel('d_{min}');
ylabel('PAPR');
title('Comparison of PAPR and Euclidean Distance / d_{min} in 16-PAM, 16-PSK, 16-QAM, and 16-CQAM');
legend('Location', 'best');
grid on;
hold off;

% Plot constellations
figure;
scatter(real(constellation_pam), imag(constellation_pam), 'filled', 'o');
title('16-PAM Constellation');
grid on;
axis equal;
for i = 1:length(constellation_pam)
    text(real(constellation_pam(i)), imag(constellation_pam(i)), ['s', num2str(i)], 'VerticalAlignment','bottom', 'HorizontalAlignment','right')
end

figure;
scatter(real(constellation_psk), imag(constellation_psk), 'filled', 'o');
title('16-PSK Constellation');
grid on;
axis equal;
for i = 1:length(constellation_psk)
    text(real(constellation_psk(i)), imag(constellation_psk(i)), ['s', num2str(i)], 'VerticalAlignment','bottom', 'HorizontalAlignment','right')
end

figure;
scatter(real(constellation_qam), imag(constellation_qam), 'filled', 'o');
title('16-QAM Constellation');
grid on;
axis equal;
for i = 1:length(constellation_qam)
    text(real(constellation_qam(i)), imag(constellation_qam(i)), ['s', num2str(i)], 'VerticalAlignment','bottom', 'HorizontalAlignment','right')
end

figure;
scatter(real(constellation_cqam), imag(constellation_cqam), 'filled', 'o');
title('16-CQAM Constellation');
grid on;
axis equal;
for i = 1:length(constellation_cqam)
    text(real(constellation_cqam(i)), imag(constellation_cqam(i)), ['s', num2str(i)], 'VerticalAlignment','bottom', 'HorizontalAlignment','right')
end

% Function to generate 16-CQAM constellation
function constellation = generate_16CQAM(M, N)
    n = M / N;
    R1 = 1; % Smallest radius
    constellation = [];
    for i = 1:N
        Ri = R1 * i; % Increasing radius for each circle
        base_angle = pi/4 * (i-1); % Rotate each circle by 45 degrees more than the previous one
        angles = (0:n-1) * (2 * pi / n) + base_angle;
        constellation = [constellation, Ri * exp(1i * angles)]; % Append points
    end
end

% Function to calculate the minimum distance (dmin) in a constellation
function dmin = calculate_dmin(constellation)
    num_points = length(constellation);
    distances = inf(num_points*(num_points-1)/2, 1);
    k = 1;
    for i = 1:num_points
        for j = i+1:num_points
            distances(k) = abs(constellation(i) - constellation(j));
            k = k + 1;
        end
    end
    dmin = min(distances);
end

% Function to normalize the average symbol energy to 1
function normalized_constellation = normalize_energy(constellation)
    avg_energy = mean(abs(constellation).^2);
    normalized_constellation = constellation / sqrt(avg_energy);
end
