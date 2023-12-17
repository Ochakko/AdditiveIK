# AdditiveIK
Software to IK motion capture as if it were brushed.  DirectX 12. FBX Input/Output. Bullet physics compatible.<br>
<br>
モーションキャプチャをブラシで盛るようにIK編集するソフト.<br>
DirectX12対応. fbx入出力. BulletPhysics対応.<br>
前作EditMot+HLSL魔導書+DirectX12魔導書というところからのリスタート.<br>
MITランセンス.<br>
<br>
# ScreenShot
![TopFig0](ScreenShots/AdditiveIK_To1001_SS1_800.png "トップ絵0")<br>
Fig.1 ScreenShot of AdditiveIK.<br>
<br>
![TopFig1](ScreenShots/AdditiveIK_To1001_HDRPBloom_1.png "トップ絵1")<br>
Fig.2 HDRP Bloom (UnityAsset JapaneseCityを読込. model:VRoid).<br>
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
# Usage<br>
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
　ジョイントマークをマウスドラッグしてIK<br>
コピーペースト<br>
　ソフトを終了しても残るコピー履歴機能により手間を削減<br>
　コピーの際にメモを記述可能で履歴画面に表示される<br>
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
　LaterTransparentプレートメニューでメッシュ内の透過テクスチャ描画順を指定
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
　DirectXTK12のライセンスはReadMe/DirectXTK12_LICENSE<br>
<br>
　DirectXTex (githubのMicrosoftのDirectXTex)<br>
　DirectXTexのライセンスはReadMe/DirectXTex_LICENSE<br>
<br>
　githubのbullet physics ver2.89 https://pybullet.org/wordpress/ <br>
　bullet physicsのライセンスはReadMe/bulletPhysics_license.txt<br>
<br>
　HLSLシェーダーの魔導書のMiniEngine https://github.com/shoeisha-books/hlsl-grimoire-sample <br>
　魔導書コードのライセンスはReadMe/hlsl-grimoire_LICENSE<br>
<br>
　DirectX12の魔導書のサンプルを使用<br>
　ライセンスはReadMe/dx12-grimoire_LICENSE<br>
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
[Description at OchakoLAB](https://ochakkolab.jp/LABMenu/MameBake3D.html "Desc-1")

# Channel
[Youtube Channel](https://www.youtube.com/@ochakkolab "Channel")

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