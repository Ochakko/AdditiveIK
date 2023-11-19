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