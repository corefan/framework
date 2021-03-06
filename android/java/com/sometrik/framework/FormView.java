package com.sometrik.framework;

import java.util.ArrayList;

import android.content.Context;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

public class FormView implements NativeMessageHandler {

  private LinearLayout baseLayout;
  private Context context;
  private ArrayList<ViewGroup> elementList;
  private ArrayList<TextView> TextElementList;
  private int viewId;
  private Handler mainHandler;
  
  public FormView(int id, Context context) {
    this.context = context;
    baseLayout = new LinearLayout(context);
    baseLayout.setOrientation(0);
    baseLayout.setId(0);
    viewId = id;
    FrameWork frame = (FrameWork)context;
    mainHandler = frame.mainHandler;
    
  }
  
  //1 = vertical, 0 = horizontal
  public void changeBaseLayoutOrientation(int orientation){
    baseLayout.setOrientation(orientation);
  }
  
  public void createButton(final int id){
    Button button = new Button(context);
    button.setId(id);
    TextElementList.add(button);
    
    button.setOnClickListener(new Button.OnClickListener() {
      @Override
      public void onClick(View arg0) {
	Message msg = Message.obtain(mainHandler, 9, id);
	msg.sendToTarget();
      }
    });
  }
  
  public void showView(){
    FrameWork frame = (FrameWork) context;
    frame.setContentView(baseLayout);
  }

  @Override
  public void handleMessage(NativeMessage message) {
    
    switch (message.getMessage()){
    case CREATE_LINEAR_LAYOUT:
      System.out.println("Formview creating layout");
      FWLayout layout = new FWLayout(context);
      layout.setId(message.getChildInternalId());
      baseLayout.addView(layout);
      break;

    case CREATE_TEXTLABEL:
      TextView textView = new TextView(context);
      textView.setId(message.getChildInternalId());
      textView.setText(message.getTextValue());
      baseLayout.addView(textView);
      break;

    case CREATE_BUTTON:
      Button button = new Button(context);
      button.setId(message.getChildInternalId());
      button.setText(message.getTextValue());
      baseLayout.addView(button);
      break;
    default:
      System.out.println("Unhandled case");
      break;

    }
    
  }

  @Override
  public int getElementId() {
    return viewId;
  }
}
