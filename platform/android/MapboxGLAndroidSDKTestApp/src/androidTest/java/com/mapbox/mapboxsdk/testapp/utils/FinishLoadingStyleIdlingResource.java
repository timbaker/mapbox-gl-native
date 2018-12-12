package com.mapbox.mapboxsdk.testapp.utils;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.WorkerThread;
import com.mapbox.mapboxsdk.maps.MapboxMap;

public class FinishLoadingStyleIdlingResource extends MapboxIdlingResource {

  @WorkerThread
  public FinishLoadingStyleIdlingResource(final Activity activity) {
    new Handler(Looper.getMainLooper()).post(() -> inflateMap(activity));
  }

  @Override
  public void initMap(MapboxMap mapboxMap) {
    super.initMap(mapboxMap);
    mapboxMap.getStyle(style -> {
      if (resourceCallback != null) {
        resourceCallback.onTransitionToIdle();
      }
    });
  }

  @Override
  public boolean isIdleNow() {
    return getMapboxMap() != null && getMapboxMap().getStyle() != null;
  }
}