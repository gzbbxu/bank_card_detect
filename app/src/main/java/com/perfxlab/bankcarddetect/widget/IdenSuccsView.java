package com.perfxlab.bankcarddetect.widget;

import android.app.Activity;
import android.content.Context;
import android.os.SystemClock;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.bumptech.glide.Glide;
import com.perfxlab.bankcarddetect.Bean.DetectInfo;
import com.perfxlab.bankcarddetect.R;

import org.reactivestreams.Subscriber;
import org.reactivestreams.Subscription;

import java.io.File;

import io.reactivex.BackpressureStrategy;
import io.reactivex.Flowable;
import io.reactivex.FlowableEmitter;
import io.reactivex.FlowableOnSubscribe;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.schedulers.Schedulers;

import static com.perfxlab.bankcarddetect.RecordActivity.BANK_CARD_DETECT;
import static com.perfxlab.bankcarddetect.RecordActivity.DRUG_BOX_DETECT;

/**
 * @author zkk
 * @date 2019/02/26
 */
public class IdenSuccsView extends FrameLayout {
    ImageView ivusericon;
    TextView tvUserName, tv_iden_fail;
    LinearLayout ll_iden_succe;
    private String lastIdenName = "";



    public IdenSuccsView(@NonNull Context context) {
        super(context);
        init();
    }

    public IdenSuccsView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public IdenSuccsView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    public void init() {
        View view = View.inflate(getContext(), R.layout.iden_succs_layout, null);
        addView(view);
        ivusericon = view.findViewById(R.id.iv_user_icon);
        tvUserName = view.findViewById(R.id.tv_id_user_name);
        tv_iden_fail = view.findViewById(R.id.tv_iden_fail);
        ll_iden_succe = view.findViewById(R.id.ll_iden_succe);
        initConsume();
    }

    FlowableEmitter flowableEmitter;

    public void initConsume() {

        Flowable<DetectInfo> flowable = Flowable.create(new FlowableOnSubscribe<DetectInfo>() {
            @Override
            public void subscribe(FlowableEmitter<DetectInfo> e) throws Exception {
//            FTLog.d(TAG, "Flowable subscribe>>:" + Thread.currentThread().getName());
                flowableEmitter = e;
            }
        }, BackpressureStrategy.LATEST);

        flowable.subscribeOn(AndroidSchedulers.mainThread()).observeOn(Schedulers.io()).subscribe(subscriber);
    }

    Subscription subscription;
    Subscriber<DetectInfo> subscriber = new Subscriber<DetectInfo>() {
        @Override
        public void onSubscribe(Subscription s) {
            s.request(Long.MAX_VALUE);
            subscription = s;
        }

        @Override
        public void onNext(DetectInfo personInfo) {
            if(personInfo.type==BANK_CARD_DETECT){
                if (!TextUtils.isEmpty(personInfo.headPath) /*&& !TextUtils.isEmpty(personInfo.headPath)*/) {
                    showIdenSucc(personInfo.headPath, "");
               /* if (personInfo.name.equals(lastIdenName)) {
                    SystemClock.sleep(200);
                    hideIdenSucc();
                } else {
                    lastIdenName = personInfo.name;
                    SystemClock.sleep(1000);
                    hideIdenSucc();
                }*/

              /*  SystemClock.sleep(150);
                hideIdenSucc();*/
                    showHideIdenFailure(true,"检测到银行卡");

                } else {
                    showHideIdenFailure(true,"未检测到結果");
                    SystemClock.sleep(500);
                    showHideIdenFailure(false,"未检测到银行卡");
                }
            }else if(personInfo.type==DRUG_BOX_DETECT){
                if (!TextUtils.isEmpty(personInfo.headPath) /*&& !TextUtils.isEmpty(personInfo.headPath)*/) {
                    showIdenSucc(personInfo.headPath, "");
               /* if (personInfo.name.equals(lastIdenName)) {
                    SystemClock.sleep(200);
                    hideIdenSucc();
                } else {
                    lastIdenName = personInfo.name;
                    SystemClock.sleep(1000);
                    hideIdenSucc();
                }*/

              /*  SystemClock.sleep(150);
                hideIdenSucc();*/
                    showHideIdenFailure(true,"检测到结果");

                } else {
                    showHideIdenFailure(true,"未检测到結果");
                    SystemClock.sleep(500);
                    showHideIdenFailure(false,"未检测到結果");
                }
            }


        }

        @Override
        public void onError(Throwable t) {
        }

        @Override
        public void onComplete() {
        }
    };

    /**
     * 添加展示队列
     * 识别失败personInfo的header 和 name 为空
     *
     * @param personInfo
     */
    public void addQueue(DetectInfo personInfo) {
        flowableEmitter.onNext(personInfo);
    }

    /**
     * 显示识别成功
     *
     * @param icon
     * @param userName
     */
    private void showIdenSucc(String icon, String userName) {
        Activity activity = (Activity) getContext();
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (isAttachedToWindow()) {
                    File file = new File(icon);
                    Glide.with(activity).load(file).into(ivusericon);
                    ll_iden_succe.setVisibility(View.VISIBLE);
                    tvUserName.setText(userName);
                }
            }
        });
    }

    /**
     * 隐藏识别成功
     */
    public void hideIdenSucc() {
        Activity activity = (Activity) getContext();
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (isAttachedToWindow()) {
                    ll_iden_succe.setVisibility(View.GONE);
                }
            }
        });
    }

    /**
     * 显示或者隐藏识别错误
     */
    public void showHideIdenFailure(boolean isShow,String text) {
        Activity activity = (Activity) getContext();

        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (isAttachedToWindow()) {
                    tv_iden_fail.setVisibility(isShow ? View.VISIBLE : View.GONE);
                    tv_iden_fail.setText(text);
                }

            }
        });

    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        if (subscription != null) {
            subscription.cancel();
        }
    }
}
