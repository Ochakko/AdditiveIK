# AdditiveIK
Software to IK motion capture as if it were brushed.  DirectX 12. FBX Input/Output. Bullet physics compatible.<br>
<br>
モーションキャプチャをブラシで盛るようにIK編集するソフト.<br>
DirectX12対応. fbx入出力. BulletPhysics対応.<br>
前作EditMot+HLSL魔導書+DirectX12魔導書というところからのリスタート.<br>
MITランセンス.<br>
<br>
# ScreenShot
![TopFig0](ScreenShots/0_AdditiveIK_1003_IndexOfMovie_0_W800.png "トップ絵0")<br>
Fig.0 New Feature of AdditiveIK.<br>
<br>
![TopFig1](ScreenShots/1009_Shaders_Annotation.png "トップ絵1")<br>
Fig.1 ver1.0.0.9以降のシェーダまとめの図 (model:VRoid).<br>
<br>
![TopFig2](ScreenShots/AdditiveIK_To1001_SS1_800.png "トップ絵2")<br>
Fig.2 ScreenShot of this Application(OnLargeWindowMode).<br>
<br>
![TopFig3](ScreenShots/AdditiveIK_To1001_HDRPBloom_1.png "トップ絵3")<br>
Fig.3 HDRP Bloom (UnityAsset JapaneseCityを読込. model:VRoid).<br>
<br>
![TopFig4](ScreenShots/AdditiveIK_1003_PracKick_2_W800.png "トップ絵4")<br>
Fig.4 Multiple Model (UnityAsset HQ Retro Farmhouseを読込. model:VRoid).<br>
<br>
![TopFig5](ScreenShots/SS_FlyingCity_4_2_W800.png "トップ絵5")<br>
Fig.5 PlayFbxCameraAnim (UnityAsset TheHuntを読込. model:VRoid).<br>
<br>
<br>
# 免責<br>
このソフトにより問題が起きたとしてもその責任は持ちません。<br>
自己責任でお願いします。<br>
<br>
<br>
# Concept<br>
モーションキャプチャを編集してゲーム用モーションに.<br>
モーションを何も無いところから作るのは難しい.<br>
それでも自分のゲームのモーションは自分で作りたい.<br>
モーキャプの動きを活かしつつ　ゲーム性をブラシで盛る.<br>
元のモーションを潰さずに盛るように編集する方法で  モーションキャプチャがゲーム用モーションになる.<br>
FBXファイルにしてUnityへ.<br>
全てのフレームにキーがあるモーションキャプチャの編集には専用ソフトが必要.<br>
AdditiveIKはそのためのソフトです.<br><br>
<br>
<br>
# Usage (使い方)<br>
入力用のfbxの準備の仕方<br>
　UnityAssetをUnityでfbxエクスポートする場合<br>
　全てのマテリアルのシェーダーをStandardにするとfbxにテクスチャ名が出力される<br>
　それ用のEditorスクリプトをみつけてきて使っています<br>
メニュー<br>
　２段構成プレートメニュー<br>
　１段目は常時固定メニュー. ２段目はカエルボタンクリックで変化.<br>
　メニューに対するGUIは右ペインウインドウに表示.<br>
IK編集<br>
　編集したいフレーム(時間横軸)範囲をロングタイムラインの四角マークをマウスで囲むようにドラッグして選択<br>
　Brushプレートメニューで編集ウェイト曲線のパラメータを設定<br>
　Brush設定によるウェイトカーブはタイムライン上に白い線で表示<br>
　ジョイントマークをマウスドラッグしてIK<br>
　元の動きを潰さずにドラッグした分だけの姿勢をウェイトを掛けて足す.<br>
　IK階層数を大きくしてもIKStop設定ジョイントでドラッグ効果は止まる.<br>
　IKStopはジョイント右クリックでオンオフ.<br>
アンドゥリドゥ<br>
　モーション編集とブラシ設定とフレーム選択に関して処理の巻き戻しと再実行が可能<br>
　アンドゥ及びリドゥボタンはCameraAndIKプレートメニューで表示<br>
　画面左上にR(数字)、W(数字)の表示がある　読込バッファ番号と書き込みバッファ番号を示す<br>
　R位置番号はアンドゥするたびに１つ減る<br>
　W位置番号は常にバッファの最後の番号<br>
　R番号W番号は　アンドゥリドゥによって　どの状態に戻ったのかを確認するためのもの<br>
高回転モードと計算スレッド数<br>
　姿勢計算はマルチスレッド化されていて描画と同時進行で計算<br>
　DispAndLimitsプレートメニューのHighRpmチェックは姿勢計算のスレッド回転を速くする<br>
　HighRpmはプレビュー時(プレイヤーボタンのプレイを押したとき)に機能する<br>
　HighRpmオン時にはプレビュー時にCPU稼働率が高くなり描画が速くなる<br>
　DispAndLimitsプレートメニューのUpdateThreadsスライダーで計算スレッド数を設定<br>
　UpdateThreadsは１モデルに対してのスレッド数<br>
　読み込みモデル数とコンピュータのCPUコア数によって速く描画できるスレッド数は異なる<br>
角度制限<br>
　読み込み済のモーションの動く範囲を可動範囲としてワンボタン設定可能<br>
　LimitEulerプレートメニューの<br>
　FromCurrentMotionボタンとFromAllRetargetedMotionでワンボタン設定<br>
　角度制限はCameraAndIKプレートメニューのLimitEulボタンでオンオフする<br>
　IK時に設定した制限角度で止まる<br>
　CameraAndIKプレートメニューのScrapingボタンオンの時には<br>
　１つでも可動軸があるとその軸に関して動く<br>
　指などの制限が厳しい設定下で動かしたい場合にScrapingを使用<br>
位置コンストレイント<br>
　ジョイント右クリックでコンストレイントジョインを指定<br>
　位置コンストレイントはIK時またはConstExecuteボタン押下時に働く<br>
　CameraAndIKプレートメニューのConstRefreshを押したときの位置にジョイントをコンスト<br>
　LimitEulをオンにして使うことを想定(無理な方向へ曲がらないように)<br>
カスタムRig<br>
　操作用メッシュを表示したいジョイントを右クリックしてCreateNewRigで作成設定<br>
　マウスのX方向Y方向それぞれのドラッグに対してXYZの度の軸を回転するかを指定<br>
　回転には符号付きの倍率を設定<br>
　RigによるIKは5階層まで伝達可能だがRigからRigも呼ぶことが出来るので５階層以上も可能<br>
　Rig操作のオンオフはCameraAndIKプレートメニューのRigボタンで行う<br>
コピーペースト<br>
　ソフトを終了しても残るコピー履歴機能により手間を削減<br>
　コピーの際にメモを記述可能で履歴画面に表示される<br>
　コピー履歴表示のソートカットボタンは上段カエルボタンを押して表示<br>
　最新を選択のチェックを外すと過去の履歴を選ぶことが出来るようになる<br>
SymCopy<br>
　対称コピー<br>
　選択フレーム範囲のモーションに対して対称コピーを行う　ペーストボタンで貼り付け<br>
　ショートカットボタンは上段カエルボタンで表示<br>
　ボタンを押して出てくるメニューでSymDirAndSymPosAndSymScale選択推奨<br>
補間<br>
　補間(Interpolation)ショートカットボタンは上段カエルボタンで表示<br>
　補間計算は選択フレーム範囲の最初と最後のフレームに対して行う<br>
　補間計算時にはBrushプレートメニューで選択したウェイト曲線を使用<br>
　通常の補間をしたい場合にはZBrushにUpCos選択 TopPosを100にして補間ボタン押下<br>
Smooth平滑化<br>
　平滑化ボタンはCameraAndIKプレートメニューにある<br>
　ボタンを右クリックで設定してから実行、左クリックは前回の設定で実行<br>
　ノイズの除去に効果がある<br>
　補間とは違って姿勢を考慮せずにオイラー角を直接計算するのでHipsに使うと体が傾くことがある<br>
物理シミュレーション<br>
　bulletPhysicsによる物理シミュレーション<br>
　RigidParamsプレートメニューで設定<br>
　Validにチェックを入れてToAllボタンで全身に剛体が入る<br>
　髪の毛ジョイントはneckジョイント選択後ToolWindowのjointマークスキップresetを押すと表示<br>
　シミュレーションしたいボーン(剛体)に対してRigidBodySimulationチェックボックスにチェック<br>
　プレイヤーボタンのグレーの再生ボタンで物理シミュ開始<br>
　BulletPhysicsプレートメニューのCalcCountスライダーで１フレーム当たりの物理計算回数を設定<br>
　CalcCountは通常１.
　大きいCalcCount値で安定することもあるが環境依存で大きくすることにより暴れることもある<br>
カメラ履歴<br>
　アセットの編集場所へすばやくカメラを移動可能<br>
　上段カエルボタンでカメラ履歴ボタンを表示<br>
　CameraAndIKプレートメニューのカメラボタンドラッグでカメラ設定後<br>
　履歴ウインドウのGetボタンで値として取得　Saveでメモ付き保存<br>
　ラジオボタン選択で履歴の場所にカメラ移動<br>
表示<br>
　DispAndLimitプレートメニューのShaderTypeでAuto, PBR, STD, NOLIGHTを切り替え可能に.<br>
　法線マップを設定していないモデルに対してPBRを選択すると　法線が０となり黒い表示になることに注意<br>
　ShaderTypeでAUTOを選んだ場合のシェーダー設定は以下<br>
　　スキンメッシュについては　[Albedo無しまたはNormal有またはMetal有]->PBR,　[Albedoだけ有またはテクスチャ１つも無し]->NOLIGHT<br>
　　非スキンメッシュについては　[Albedo無しまたはNormal有またはMetal有]->PBR,　[Albedoだけ有またはテクスチャ１つも無し]->STD<br>
　スキンメッシュと非スキンメッシュそれぞれのShaderTypeに対して　不透明、半透明、半透明常時上書きの設定があり自動設定<br>
　DispGroupプレートメニューでメッシュごとにグループ指定<br>
　　設定画面としてはモデルパネルで選択したモデル単位<br>
　　複数モデルを横断してグループ番号の小さい順に描画<br>
　　グループ番号１以外は強制的に半透明として描画<br>
　　メッシュ右クリックから類似する名前のメッシュの一括処理が可能<br>
　LaterTransparentプレートメニューでメッシュ内の透過テクスチャ描画順を指定<br>
　Shaderプレートメニュー<br>
　　マテリアル名ボタン押下で設定、見出しボタン押下で選択モデルの全マテリアルに対して設定<br>
　　シェーダータイプとしてAuto, PBR, Std., NoLightの４種類から選択<br>
　　PBRは法線テクスチャまたはメタルスムーステクスチャのどちらかが設定されている場合を想定<br>
　　Autoを選ぶと設定具合をみて自動的にマテリアルごとにPBR, Std.,NoLightを振り分け<br>
　Shadowプレートメニュー<br>
　　デプスシャドウです　セルフシャドウも表示されます<br>
　　影の向きはLightプレートメニューで設定したライトの番号で指定(1から8までのボタン)<br>
　　シャドウ用のカメラの位置から上記ライトの向きに影は落ちます<br>
　　シャドウライト位置と対象物間に遮るオブジェクトがある場合plus up設定でライトを持ち上げる<br>
　　影の明るさは0.0で真っ黒、1.0でdiffuse色<br>
　　シャドウカメラの位置設定にはカメラのターゲット位置が関係する<br>
　　ジョイントを選択後にDispAndLimitsメニューのLockCameraToSelectedJointチェックで
　　選択したジョイントの位置をカメラターゲット位置にすることが可能<br>
　　カメラのターゲット位置によっても影の落ち方は変わる<br>
　HDRP<br>
　　Lightプレートメニューで設定可能なライトの強さは1.0よりも大きい3.0.<br>
　　Shaderプレートメニューで設定可能なマテリアルごとのライト倍率最大値は5.0.<br>
　　SahderでStd.を選んで　ライトを1.0より大きくすると　自動的にBloomが掛かる.<br>
　bvh2Fbxバッチ<br>
　　bvhファイルをfbxファイルに変換.<br>
　　扱うことが出来るbvhのタイプは限定的(回転順序には対応.軸の違いには未対応).<br>
　　CMU(カーネギーメロン大学)のMotionBruilderFriendlyタイプのサンプルでテスト.<br>
　retargetバッチ<br>
　　指定フォルダ内のfbxを選択中のモデルにリターゲット<br>
　　モーションのボーン名とモデルのボーン名の対応表*.rtg(テキストファイル)が必要<br>
　　Test/RetargetSettingFileExampleフォルダに*.rtgの例を同梱<br>
　　Testフォルダの*.rtgのファイル名は Model名_モーション種類.rtg<br>
　　指定フォルダにretarget.rtgという名前で*.rtgファイルを置く必要有<br>
<br>
<br>
<br>
# UpdateLog<br>
[UpdateLog.txt](UpdateLog.txt)<br>
<br>
<br>
<br>
# 開発環境(ソフトウェア)<br>
　fbxを扱うにはMayaが必須と言えます<br>
　Mayaは趣味の人？に対しては安価で？提供されている<br>
　（インディー契約の条件については以下のページ）<br>
　https://area.autodesk.jp/product/maya-3ds-max-indie/<br>
<br>
<br>
　開発環境(2023/11/12時点)を書きます。<br>
　OS Windows11 Pro<br>
　VisualStudio2022Pro(VisualStudioSubscription)<br>
　MayaIndie(Subscription FBXの確認はMayaでする)<br>
　Unity3D 2022LTS(2022.3.3)<br>
　プロが作ったアセットをUnityAssetStoreでゲットしてUnity3Dでfbx出力 https://assetstore.unity.com/?locale=ja-JP <br>
　FBXSDKは2020.3.4(VC2022) https://www.autodesk.com/products/fbx/ <br>
　Microsoft MultiThreadLibrary PPL.<br>
<br>
　DirectX12 (githubのMicrosoftのDirectXTK12)<br>
　DirectXTK12のライセンスは3rd_LICENSE/DirectXTK12_LICENSE<br>
<br>
　DirectXTex (githubのMicrosoftのDirectXTex)<br>
　DirectXTexのライセンスは3rd_LICENSE/DirectXTex_LICENSE<br>
<br>
　githubのbullet physics ver2.89 https://pybullet.org/wordpress/ <br>
　bullet physicsのライセンスは3rd_LICENSE/bulletPhysics_license.txt<br>
<br>
　HLSLシェーダーの魔導書のMiniEngine https://github.com/shoeisha-books/hlsl-grimoire-sample <br>
　魔導書コードのライセンスは3rd_LICENSE/hlsl-grimoire_LICENSE<br>
<br>
　DirectX12の魔導書のサンプルを使用<br>
　ライセンスは3rd_LICENSE/dx12-grimoire_LICENSE<br>
<br>
　MicrosoftのDXUT11のDXUTTimerを使っています.<br>
<br>
<br>
　Testディレクトリのモデル作成にVRoidStudioを使用します<br>
　https://vroid.com/studio<br>
<br>
<br>
# 開発環境(ハードウェア)<br>
古めの８コアIntelマシン<br>
　GeForce RTX 2080 8GB<br>
　128GB RAM<br>
　1T SSD<br>
<br>
<br>
# Description
[Description at OchakoLAB](https://ochakkolab.jp/LABMenu/AdditiveIK.html "Desc-1")

# Explaining Movie
[Movie About New Feature](https://youtu.be/oKGY62YosSg "About New Feature")

[Movie About Usage Example](https://youtu.be/R0MsXt8plYg "Usage example")

# Site
[OchakkoLAB](https://ochakkolab.jp/ "OchakkoLAB")
<br>
<br>
<br>
<br>
<br>
<br>
メール：info@ochakkolab.jp<br>
Author: おちゃっこ@おちゃっこLAB<br>
<br>
<br>
<br>
<br>
<br>
<br>