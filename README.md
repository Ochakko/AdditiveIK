# AdditiveIK
Software to IK motion capture as if it were brushed.  DirectX 12. FBX Input/Output. Bullet physics compatible.<br><br>

このソフトにより問題が起きたとしてもその責任は持ちません。
自己責任でお願いします。


2023_11_12<br>
まずはAdditiveIKのレポジトリを作成<br>
MameBake3D + HLSシェーダーの魔導書(DirectX12)の描画　+ DirectX12の魔導書のDX12文法 + bullet physicsからスタートする予定<br>
ビルドは出来ますがまだ動きません　これからです.<br>
<br>
<br>
2023_11_14<br>
もの凄い作り直しをしようとしていましたが　考えた結果　表示とUIの挿げ替えをすることに<br>
MameBake3Dの表示をPBRとカスケードシャドウに対応させて　UIはimguiで表示するための作業をしていく 予定<br>
とりあえずDX12用のファイルを足して　DX11をコメントアウト　してビルドして起動可能に<br>
まだモデルを表示できない状態<br>
<br>
<br>
2023/11/18<br>
DirectX12による表示開始<br>
とりあえず　白いシルエットでモデルを表示<br>
VRoidモデルはシルエット表示できた<br>
仕組みとしてはテクスチャに対応しているのだが　シェーダーでテクスチャを使うと何も表示されなかった<br>
4000個くらいメッシュがあるfbxを読み込むとアウトオブメモリエラーだった<br>
次回はテクスチャを表示したい<br>
<br>
<br>
2023/11/19<br>
Texture表示が可能になった<br>
透過テクスチャを有効に<br>
透過テクスチャ遅延描画順機能(LaterTransparent)を有効に<br>
<br>
<br>
2023/11/20<br>
Texture設定の無いメッシュ描画時には白いalbedoテクスチャを使用<br>
マテリアルのdiffuse色のテクスチャを作成してalbedoテクスチャと乗算して描画<br>
このようにすることで　古いマテリアル形式と将来対応するPBRとが共存出来る可能性<br>
4KTV接続時にアプリウインドウ大と小を選べるように<br>
<br>
<br>
2023/11/22<br>
スプライトGUIを魔導書のSpriteクラスで.<br>
プロジェクトやfbxを読み込むとプレートメニューが出ます.<br>
下の方のカエルボタンでメニュー変更. 上の方のカエルボタンでショートカットボタンを変更.<br>
Documents/ModifyAboutGrimoireSpriteClass.docx追加<br>
Documents/CoexistenceOfConventionalMaterialsAndPBR.docx追加<br>
カメラ操作を有効に. CameraAndIKプレートをクリックしてボタンを出し、３つのカメラボタンドラッグでカメラ操作.<br>
モーションのプレビューが出来るようになりました.<br>
モーション付きモデルを読み込み後にタイムラインのプレイマークのボタンでプレビュー.<br>
<br>
<br>
2023/11/23<br>
デバッグビルドで実行終了時に表示されるメモリリークが気になってリファクタリング<br>
メモリリークは無くなった?<br>
<br>
<br>
2023/11/25<br>
魔導書のSpriteクラスを拡張してインスタンシングに対応させたInstancedSpriteクラス新規<br>
同じスプライトを違う場所違う大きさで描画可能<br>
InstancedSpriteでfpsとアンドゥバッファ位置を表示<br>
GUIに関しては現在のウインドウに重ねて表示できるものがみつからないため　右ペインに通常通り表示する予定に変更<br>
なるべくクリック回数が増えないように段階を追って改善していくつもり<br>
関節部ドラッグでとりあえずAdditiveIK出来るようにしました.　ブラシパラメータ設定などはGUIを準備してから.<br>
<br>
<br>
2023/11/26<br>
選択ジョイントの位置にマニピュレータを表示<br>
マウスホバーでマニピュレータをハイライト表示<br>
物理プレイボタンと物理ベイクボタンをプレイヤーボタンに追加<br>
レンダリングエンジンにスプライト描画処理を追加.<br>
ジョイント位置にスプライト表示. マークは白、セレクトツリーは赤.<br>
<br>
<br>
2023/11/27<br>
レンダリングエンジンにフォント表示を追加<br>
ツールチップを表示<br>
線だけで構成されたメッシュの表示にも対応<br>
地面データとして線で出来た格子と軸を表示<br>
<br>
<br>
2023/11/28<br>
プレートメニューを２段構成に.<br>
１段目は常時固定メニュー. ２段目はカエルボタンで変化.<br>
１段目のメニューに対するGUIは右ペインウインドウに表示.<br>
１段目と２段目のメニューに関わる作業を行き来するに便利なようにしたつもり.<br>
１段目のCameraAndIKは他のメニューとは独立してトグル.<br>
１段目のCameraAndIK以外は１段目の他のメニューをオンにしたときにオフになる.<br>
２段目のプレートをオンにすると１段目のCameraAndIK以外はオフになる.<br>
<br>
<br>
2023/11/29<br>
DescriptorHeapに関するリファクタリング<br>
大きい背景読み込み時に4038個目のDescriptorHeap作成でE_OUTOFMEMORYになる症状は変わらず<br>
背景の描画をきれいにしたかったのだが　どうするか試しながら考える<br>
<br>
<br>
2023/11/30<br>
MaterialBankを追加することにより大きい背景fbxも読み込めるようになった<br>
ただし１つのfbxにつきボーン数は1000個までという制限付き<br>
デスクリプタヒープの数を減らすことにより背景を読めるようになったのだが<br>
マテリアルごとに全部のボーン姿勢を転送しなくてはならなくなり重くなってしまった<br>
マニピュレータの常時上書き設定は一時的に効かなくしている<br>
明日は背景を綺麗に表示するために修正する予定(予定)<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
サイト：https://ochakkolab.jp<br>
メール：info@ochakkolab.jp<br>
Author: おちゃっこ@おちゃっこLAB<br>
<br>
<br>
<br>
<br>
<br>
<br>
#### 動作環境 　ここから　####<br>
<br>
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
<br>
参考までに　開発ハードウェア(2023/11/13時点)も書いておきます<br>
古めの８コアIntelマシン<br>
GeForce RTX 2080 8GB<br>
128GB RAM<br>
1T SSD<br>
<br>
<br>
<br>
<br>