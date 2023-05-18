package com.example.deskbuddyapp;

import android.graphics.drawable.AnimationDrawable;
import androidx.constraintlayout.widget.ConstraintLayout;


/** This helper class initializes the background animation used as backgrounds in the app. */
public class BackgroundAnimator {

    public void animateBackground(ConstraintLayout constraintLayout) {
        AnimationDrawable animationDrawable = (AnimationDrawable) constraintLayout.getBackground();

        // Set the duration in between gradients.
        animationDrawable.setEnterFadeDuration(2500);
        animationDrawable.setExitFadeDuration(5000);
        animationDrawable.start();
    }

}
