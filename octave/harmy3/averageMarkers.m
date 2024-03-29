function R = averageMarkers(markers, stepSize, numFrames)
% R = averageMarkers(markers, stepSize, numFrames)
%
% Calculate average marker lengthes per frame

% Number of markers that were already written into that frame:
%numEntries = zeros(numFrames);
% The currently calculated average value.

% Cell array filled with the diff entries of the markers.
markerDiffs = cell(numFrames, 1);

for k = 1:length(markers)-1
  markerDiff = markers(k+1) - markers(k);
  if (markerDiff < 2 * stepSize)
    markerStart = floor(markers(k) / stepSize) + 1;
    markerEnd = ceil(markers(k+1) / stepSize) + 1;
    markerDiffs{markerStart} = [markerDiffs{markerStart}; markerDiff];
    markerDiffs{markerEnd} = [markerDiffs{markerEnd}; markerDiff];
  end
end

R = zeros(numFrames);
for k = 1:numFrames
  if (length(markerDiffs{k}) > 0)
    R(k) = median(markerDiffs{k});
  end
end

% Copy from left to right
for k = 2:numFrames
  if (R(k) == 0)
    R(k) = R(k-1);
  end
end
% Copy from right to left
for k = numFrames-1:-1:1
  if (R(k) == 0)
    R(k) = R(k+1);
  end
end