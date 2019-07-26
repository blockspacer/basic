// Copyright 2018 The Basic Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "basic/hash/internal/city.h"

#include <string.h>
#include <cstdio>
#include <iostream>
#include "gtest/gtest.h"

namespace basic {
namespace hash_internal {

static const uint64_t k0 = 0xc3a5c85c97cb3127ULL;
static const uint64_t kSeed0 = 1234567;
static const uint64_t kSeed1 = k0;
static const int kDataSize = 1 << 20;
static const int kTestSize = 300;

static char data[kDataSize];

// Initialize data to pseudorandom values.
void setup() {
  uint64_t a = 9;
  uint64_t b = 777;
  for (int i = 0; i < kDataSize; i++) {
    a += b;
    b += a;
    a = (a ^ (a >> 41)) * k0;
    b = (b ^ (b >> 41)) * k0 + i;
    uint8_t u = b >> 37;
    memcpy(data + i, &u, 1);  // uint8_t -> char
  }
}

#define C(x) 0x##x##ULL
static const uint64_t testdata[kTestSize][4] = {
    {C(9ae16a3b2f90404f), C(75106db890237a4a), C(3feac5f636039766),
     C(dc56d17a)},
    {C(541150e87f415e96), C(1aef0d24b3148a1a), C(bacc300e1e82345a),
     C(99929334)},
    {C(f3786a4b25827c1), C(34ee1a2bf767bd1c), C(2f15ca2ebfb631f2), C(4252edb7)},
    {C(ef923a7a1af78eab), C(79163b1e1e9a9b18), C(df3b2aca6e1e4a30),
     C(ebc34f3c)},
    {C(11df592596f41d88), C(843ec0bce9042f9c), C(cce2ea1e08b1eb30),
     C(26f2b463)},
    {C(831f448bdc5600b3), C(62a24be3120a6919), C(1b44098a41e010da),
     C(b042c047)},
    {C(3eca803e70304894), C(d80de767e4a920a), C(a51cfbb292efd53d), C(e73bb0a8)},
    {C(1b5a063fb4c7f9f1), C(318dbc24af66dee9), C(10ef7b32d5c719af),
     C(91dfdd75)},
    {C(a0f10149a0e538d6), C(69d008c20f87419f), C(41b36376185b3e9e),
     C(c87f95de)},
    {C(fb8d9c70660b910b), C(a45b0cc3476bff1b), C(b28d1996144f0207),
     C(3f5538ef)},
    {C(236827beae282a46), C(e43970221139c946), C(4f3ac6faa837a3aa),
     C(70eb1a1f)},
    {C(c385e435136ecf7c), C(d9d17368ff6c4a08), C(1b31eed4e5251a67),
     C(cfd63b83)},
    {C(e3f6828b6017086d), C(21b4d1900554b3b0), C(bef38be1809e24f1),
     C(894a52ef)},
    {C(851fff285561dca0), C(4d1277d73cdf416f), C(28ccffa61010ebe2),
     C(9cde6a54)},
    {C(61152a63595a96d9), C(d1a3a91ef3a7ba45), C(443b6bb4a493ad0c),
     C(6c4898d5)},
    {C(44473e03be306c88), C(30097761f872472a), C(9fd1b669bfad82d7),
     C(13e1978e)},
    {C(3ead5f21d344056), C(fb6420393cfb05c3), C(407932394cbbd303), C(51b4ba8)},
    {C(6abbfde37ee03b5b), C(83febf188d2cc113), C(cda7b62d94d5b8ee),
     C(b6b06e40)},
    {C(943e7ed63b3c080), C(1ef207e9444ef7f8), C(ef4a9f9f8c6f9b4a), C(240a2f2)},
    {C(d72ce05171ef8a1a), C(c6bd6bd869203894), C(c760e6396455d23a),
     C(5dcefc30)},
    {C(4182832b52d63735), C(337097e123eea414), C(b5a72ca0456df910),
     C(7a48b105)},
    {C(d6cdae892584a2cb), C(58de0fa4eca17dcd), C(43df30b8f5f1cb00),
     C(fd55007b)},
    {C(5c8e90bc267c5ee4), C(e9ae044075d992d9), C(f234cbfd1f0a1e59),
     C(6b95894c)},
    {C(bbd7f30ac310a6f3), C(b23b570d2666685f), C(fb13fb08c9814fe7),
     C(3360e827)},
    {C(36a097aa49519d97), C(8204380a73c4065), C(77c2004bdd9e276a), C(45177e0b)},
    {C(dc78cb032c49217), C(112464083f83e03a), C(96ae53e28170c0f5), C(7c6fffe4)},
    {C(441593e0da922dfe), C(936ef46061469b32), C(204a1921197ddd87),
     C(bbc78da4)},
    {C(2ba3883d71cc2133), C(72f2bbb32bed1a3c), C(27e1bd96d4843251),
     C(c5c25d39)},
    {C(f2b6d2adf8423600), C(7514e2f016a48722), C(43045743a50396ba),
     C(b6e5d06e)},
    {C(38fffe7f3680d63c), C(d513325255a7a6d1), C(31ed47790f6ca62f),
     C(6178504e)},
    {C(b7477bf0b9ce37c6), C(63b1c580a7fd02a4), C(f6433b9f10a5dac), C(bd4c3637)},
    {C(55bdb0e71e3edebd), C(c7ab562bcf0568bc), C(43166332f9ee684f),
     C(6e7ac474)},
    {C(782fa1b08b475e7), C(fb7138951c61b23b), C(9829105e234fb11e), C(1fb4b518)},
    {C(c5dc19b876d37a80), C(15ffcff666cfd710), C(e8c30c72003103e2),
     C(31d13d6d)},
    {C(5e1141711d2d6706), C(b537f6dee8de6933), C(3af0a1fbbe027c54),
     C(26fa72e3)},
    {C(782edf6da001234f), C(f48cbd5c66c48f3), C(808754d1e64e2a32), C(6a7433bf)},
    {C(d26285842ff04d44), C(8f38d71341eacca9), C(5ca436f4db7a883c),
     C(4e6df758)},
    {C(c6ab830865a6bae6), C(6aa8e8dd4b98815c), C(efe3846713c371e5),
     C(d57f63ea)},
    {C(44b3a1929232892), C(61dca0e914fc217), C(a607cc142096b964), C(52ef73b3)},
    {C(4b603d7932a8de4f), C(fae64c464b8a8f45), C(8fafab75661d602a), C(3cb36c3)},
    {C(4ec0b54cf1566aff), C(30d2c7269b206bf4), C(77c22e82295e1061),
     C(72c39bea)},
    {C(ed8b7a4b34954ff7), C(56432de31f4ee757), C(85bd3abaa572b155),
     C(a65aa25c)},
    {C(5d28b43694176c26), C(714cc8bc12d060ae), C(3437726273a83fe6),
     C(74740539)},
    {C(6a1ef3639e1d202e), C(919bc1bd145ad928), C(30f3f7e48c28a773),
     C(c3ae3c26)},
    {C(159f4d9e0307b111), C(3e17914a5675a0c), C(af849bd425047b51), C(f29db8a2)},
    {C(cc0a840725a7e25b), C(57c69454396e193a), C(976eaf7eee0b4540),
     C(1ef4cbf4)},
    {C(a2b27ee22f63c3f1), C(9ebde0ce1b3976b2), C(2fe6a92a257af308),
     C(a9be6c41)},
    {C(d8f2f234899bcab3), C(b10b037297c3a168), C(debea2c510ceda7f), C(fa31801)},
    {C(584f28543864844f), C(d7cee9fc2d46f20d), C(a38dca5657387205),
     C(8331c5d8)},
    {C(a94be46dd9aa41af), C(a57e5b7723d3f9bd), C(34bf845a52fd2f), C(e9876db8)},
    {C(9a87bea227491d20), C(a468657e2b9c43e7), C(af9ba60db8d89ef7),
     C(27b0604e)},
    {C(27688c24958d1a5c), C(e3b4a1c9429cf253), C(48a95811f70d64bc),
     C(dcec07f2)},
    {C(5d1d37790a1873ad), C(ed9cd4bcc5fa1090), C(ce51cde05d8cd96a),
     C(cff0a82a)},
    {C(1f03fd18b711eea9), C(566d89b1946d381a), C(6e96e83fc92563ab),
     C(fec83621)},
    {C(f0316f286cf527b6), C(f84c29538de1aa5a), C(7612ed3c923d4a71), C(743d8dc)},
    {C(297008bcb3e3401d), C(61a8e407f82b0c69), C(a4a35bff0524fa0e),
     C(64d41d26)},
    {C(43c6252411ee3be), C(b4ca1b8077777168), C(2746dc3f7da1737f), C(acd90c81)},
    {C(ce38a9a54fad6599), C(6d6f4a90b9e8755e), C(c3ecc79ff105de3f),
     C(7c746a4b)},
    {C(270a9305fef70cf), C(600193999d884f3a), C(f4d49eae09ed8a1), C(b1047e99)},
    {C(e71be7c28e84d119), C(eb6ace59932736e6), C(70c4397807ba12c5),
     C(d1fd1068)},
    {C(b5b58c24b53aaa19), C(d2a6ab0773dd897f), C(ef762fe01ecb5b97),
     C(56486077)},
    {C(44dd59bd301995cf), C(3ccabd76493ada1a), C(540db4c87d55ef23),
     C(6069be80)},
    {C(b4d4789eb6f2630b), C(bf6973263ce8ef0e), C(d1c75c50844b9d3), C(2078359b)},
    {C(12807833c463737c), C(58e927ea3b3776b4), C(72dd20ef1c2f8ad0),
     C(9ea21004)},
    {C(e88419922b87176f), C(bcf32f41a7ddbf6f), C(d6ebefd8085c1a0f),
     C(9c9cfe88)},
    {C(105191e0ec8f7f60), C(5918dbfcca971e79), C(6b285c8a944767b9),
     C(b70a6ddd)},
    {C(a5b88bf7399a9f07), C(fca3ddfd96461cc4), C(ebe738fdc0282fc6),
     C(dea37298)},
    {C(d08c3f5747d84f50), C(4e708b27d1b6f8ac), C(70f70fd734888606),
     C(8f480819)},
    {C(2f72d12a40044b4b), C(889689352fec53de), C(f03e6ad87eb2f36), C(30b3b16)},
    {C(aa1f61fdc5c2e11e), C(c2c56cd11277ab27), C(a1e73069fdf1f94f),
     C(f31bc4e8)},
    {C(9489b36fe2246244), C(3355367033be74b8), C(5f57c2277cbce516),
     C(419f953b)},
    {C(358d7c0476a044cd), C(e0b7b47bcbd8854f), C(ffb42ec696705519),
     C(20e9e76d)},
    {C(b0c48df14275265a), C(9da4448975905efa), C(d716618e414ceb6d),
     C(646f0ff8)},
    {C(daa70bb300956588), C(410ea6883a240c6d), C(f5c8239fb5673eb3),
     C(eeb7eca8)},
    {C(4ec97a20b6c4c7c2), C(5913b1cd454f29fd), C(a9629f9daf06d685), C(8112bb9)},
    {C(5c3323628435a2e8), C(1bea45ce9e72a6e3), C(904f0a7027ddb52e),
     C(85a6d477)},
    {C(c1ef26bea260abdb), C(6ee423f2137f9280), C(df2118b946ed0b43),
     C(56f76c84)},
    {C(6be7381b115d653a), C(ed046190758ea511), C(de6a45ffc3ed1159),
     C(9af45d55)},
    {C(ae3eece1711b2105), C(14fd3f4027f81a4a), C(abb7e45177d151db),
     C(d1c33760)},
    {C(376c28588b8fb389), C(6b045e84d8491ed2), C(4e857effb7d4e7dc),
     C(c56bbf69)},
    {C(58d943503bb6748f), C(419c6c8e88ac70f6), C(586760cbf3d3d368),
     C(abecfb9b)},
    {C(dfff5989f5cfd9a1), C(bcee2e7ea3a96f83), C(681c7874adb29017),
     C(8de13255)},
    {C(7fb19eb1a496e8f5), C(d49e5dfdb5c0833f), C(c0d5d7b2f7c48dc7),
     C(a98ee299)},
    {C(5dba5b0dadccdbaa), C(4ba8da8ded87fcdc), C(f693fdd25badf2f0),
     C(3015f556)},
    {C(688bef4b135a6829), C(8d31d82abcd54e8e), C(f95f8a30d55036d7),
     C(5a430e29)},
    {C(d8323be05433a412), C(8d48fa2b2b76141d), C(3d346f23978336a5),
     C(2797add0)},
    {C(3b5404278a55a7fc), C(23ca0b327c2d0a81), C(a6d65329571c892c),
     C(27d55016)},
    {C(2a96a3f96c5e9bbc), C(8caf8566e212dda8), C(904de559ca16e45e),
     C(84945a82)},
    {C(22bebfdcc26d18ff), C(4b4d8dcb10807ba1), C(40265eee30c6b896),
     C(3ef7e224)},
    {C(627a2249ec6bbcc2), C(c0578b462a46735a), C(4974b8ee1c2d4f1f),
     C(35ed8dc8)},
    {C(3abaf1667ba2f3e0), C(ee78476b5eeadc1), C(7e56ac0a6ca4f3f4), C(6a75e43d)},
    {C(3931ac68c5f1b2c9), C(efe3892363ab0fb0), C(40b707268337cd36),
     C(235d9805)},
    {C(b98fb0606f416754), C(46a6e5547ba99c1e), C(c909d82112a8ed2), C(f7d69572)},
    {C(7f7729a33e58fcc4), C(2e4bc1e7a023ead4), C(e707008ea7ca6222),
     C(bacd0199)},
    {C(42a0aa9ce82848b3), C(57232730e6bee175), C(f89bb3f370782031),
     C(e428f50e)},
    {C(6b2c6d38408a4889), C(de3ef6f68fb25885), C(20754f456c203361),
     C(81eaaad3)},
    {C(930380a3741e862a), C(348d28638dc71658), C(89dedcfd1654ea0d),
     C(addbd3e3)},
    {C(94808b5d2aa25f9a), C(cec72968128195e0), C(d9f4da2bdc1e130f),
     C(e66dbca0)},
    {C(b31abb08ae6e3d38), C(9eb9a95cbd9e8223), C(8019e79b7ee94ea9),
     C(afe11fd5)},
    {C(dccb5534a893ea1a), C(ce71c398708c6131), C(fe2396315457c164),
     C(a71a406f)},
    {C(6369163565814de6), C(8feb86fb38d08c2f), C(4976933485cc9a20),
     C(9d90eaf5)},
    {C(edee4ff253d9f9b3), C(96ef76fb279ef0ad), C(a4d204d179db2460),
     C(6665db10)},
    {C(941993df6e633214), C(929bc1beca5b72c6), C(141fc52b8d55572d),
     C(9c977cbf)},
    {C(859838293f64cd4c), C(484403b39d44ad79), C(bf674e64d64b9339),
     C(ee83ddd4)},
    {C(c19b5648e0d9f555), C(328e47b2b7562993), C(e756b92ba4bd6a51), C(26519cc)},
    {C(f963b63b9006c248), C(9e9bf727ffaa00bc), C(c73bacc75b917e3a),
     C(a485a53f)},
    {C(6a8aa0852a8c1f3b), C(c8f1e5e206a21016), C(2aa554aed1ebb524),
     C(f62bc412)},
    {C(740428b4d45e5fb8), C(4c95a4ce922cb0a5), C(e99c3ba78feae796),
     C(8975a436)},
    {C(658b883b3a872b86), C(2f0e303f0f64827a), C(975337e23dc45e1), C(94ff7f41)},
    {C(6df0a977da5d27d4), C(891dd0e7cb19508), C(fd65434a0b71e680), C(760aa031)},
    {C(a900275464ae07ef), C(11f2cfda34beb4a3), C(9abf91e5a1c38e4), C(3bda76df)},
    {C(810bc8aa0c40bcb0), C(448a019568d01441), C(f60ec52f60d3aeae),
     C(498e2e65)},
    {C(22036327deb59ed7), C(adc05ceb97026a02), C(48bff0654262672b),
     C(d38deb48)},
    {C(7d14dfa9772b00c8), C(595735efc7eeaed7), C(29872854f94c3507),
     C(82b3fb6b)},
    {C(2d777cddb912675d), C(278d7b10722a13f9), C(f5c02bfb7cc078af),
     C(e500e25f)},
    {C(f2ec98824e8aa613), C(5eb7e3fb53fe3bed), C(12c22860466e1dd4),
     C(bd2bb07c)},
    {C(5e763988e21f487f), C(24189de8065d8dc5), C(d1519d2403b62aa0),
     C(3a2b431d)},
    {C(48949dc327bb96ad), C(e1fd21636c5c50b4), C(3f6eb7f13a8712b4),
     C(7322a83d)},
    {C(b7c4209fb24a85c5), C(b35feb319c79ce10), C(f0d3de191833b922),
     C(a645ca1c)},
    {C(9c9e5be0943d4b05), C(b73dc69e45201cbb), C(aab17180bfe5083d),
     C(8909a45a)},
    {C(3898bca4dfd6638d), C(f911ff35efef0167), C(24bdf69e5091fc88),
     C(bd30074c)},
    {C(5b5d2557400e68e7), C(98d610033574cee), C(dfd08772ce385deb), C(c17cf001)},
    {C(a927ed8b2bf09bb6), C(606e52f10ae94eca), C(71c2203feb35a9ee),
     C(26ffd25a)},
    {C(8d25746414aedf28), C(34b1629d28b33d3a), C(4d5394aea5f82d7b),
     C(f1d8ce3c)},
    {C(b5bbdb73458712f2), C(1ff887b3c2a35137), C(7f7231f702d0ace9),
     C(3ee8fb17)},
    {C(3d32a26e3ab9d254), C(fc4070574dc30d3a), C(f02629579c2b27c9),
     C(a77acc2a)},
    {C(9371d3c35fa5e9a5), C(42967cf4d01f30), C(652d1eeae704145c), C(f4556dee)},
    {C(cbaa3cb8f64f54e0), C(76c3b48ee5c08417), C(9f7d24e87e61ce9), C(de287a64)},
    {C(b2e23e8116c2ba9f), C(7e4d9c0060101151), C(3310da5e5028f367),
     C(878e55b9)},
    {C(8aa77f52d7868eb9), C(4d55bd587584e6e2), C(d2db37041f495f5), C(7648486)},
    {C(858fea922c7fe0c3), C(cfe8326bf733bc6f), C(4e5e2018cf8f7dfc),
     C(57ac0fb1)},
    {C(46ef25fdec8392b1), C(e48d7b6d42a5cd35), C(56a6fe1c175299ca),
     C(d01967ca)},
    {C(8d078f726b2df464), C(b50ee71cdcabb299), C(f4af300106f9c7ba),
     C(96ecdf74)},
    {C(35ea86e6960ca950), C(34fe1fe234fc5c76), C(a00207a3dc2a72b7),
     C(779f5506)},
    {C(8aee9edbc15dd011), C(51f5839dc8462695), C(b2213e17c37dca2d),
     C(3c94c2de)},
    {C(c3e142ba98432dda), C(911d060cab126188), C(b753fbfa8365b844),
     C(39f98faf)},
    {C(123ba6b99c8cd8db), C(448e582672ee07c4), C(cebe379292db9e65),
     C(7af31199)},
    {C(ba87acef79d14f53), C(b3e0fcae63a11558), C(d5ac313a593a9f45),
     C(e341a9d6)},
    {C(bcd3957d5717dc3), C(2da746741b03a007), C(873816f4b1ece472), C(ca24aeeb)},
    {C(61442ff55609168e), C(6447c5fc76e8c9cf), C(6a846de83ae15728),
     C(b2252b57)},
    {C(dbe4b1b2d174757f), C(506512da18712656), C(6857f3e0b8dd95f), C(72c81da1)},
    {C(531e8e77b363161c), C(eece0b43e2dae030), C(8294b82c78f34ed1),
     C(6b9fce95)},
    {C(f71e9c926d711e2b), C(d77af2853a4ceaa1), C(9aa0d6d76a36fae7),
     C(19399857)},
    {C(cb20ac28f52df368), C(e6705ee7880996de), C(9b665cc3ec6972f2),
     C(3c57a994)},
    {C(e4a794b4acb94b55), C(89795358057b661b), C(9c4cdcec176d7a70),
     C(c053e729)},
    {C(cb942e91443e7208), C(e335de8125567c2a), C(d4d74d268b86df1f),
     C(51cbbba7)},
    {C(ecca7563c203f7ba), C(177ae2423ef34bb2), C(f60b7243400c5731),
     C(1acde79a)},
    {C(1652cb940177c8b5), C(8c4fe7d85d2a6d6d), C(f6216ad097e54e72),
     C(2d160d13)},
    {C(31fed0fc04c13ce8), C(3d5d03dbf7ff240a), C(727c5c9b51581203),
     C(787f5801)},
    {C(e7b668947590b9b3), C(baa41ad32938d3fa), C(abcbc8d4ca4b39e4),
     C(c9629828)},
    {C(1de2119923e8ef3c), C(6ab27c096cf2fe14), C(8c3658edca958891),
     C(be139231)},
    {C(1269df1e69e14fa7), C(992f9d58ac5041b7), C(e97fcf695a7cbbb4),
     C(7df699ef)},
    {C(820826d7aba567ff), C(1f73d28e036a52f3), C(41c4c5a73f3b0893),
     C(8ce6b96d)},
    {C(ffe0547e4923cef9), C(3534ed49b9da5b02), C(548a273700fba03d),
     C(6f9ed99c)},
    {C(72da8d1b11d8bc8b), C(ba94b56b91b681c6), C(4e8cc51bd9b0fc8c),
     C(e0244796)},
    {C(d62ab4e3f88fc797), C(ea86c7aeb6283ae4), C(b5b93e09a7fe465), C(4ccf7e75)},
    {C(d0f06c28c7b36823), C(1008cb0874de4bb8), C(d6c7ff816c7a737b),
     C(915cef86)},
    {C(99b7042460d72ec6), C(2a53e5e2b8e795c2), C(53a78132d9e1b3e3),
     C(5cb59482)},
    {C(4f4dfcfc0ec2bae5), C(841233148268a1b8), C(9248a76ab8be0d3), C(6ca3f532)},
    {C(fe86bf9d4422b9ae), C(ebce89c90641ef9c), C(1c84e2292c0b5659),
     C(e24f3859)},
    {C(a90d81060932dbb0), C(8acfaa88c5fbe92b), C(7c6f3447e90f7f3f),
     C(adf5a9c7)},
    {C(17938a1b0e7f5952), C(22cadd2f56f8a4be), C(84b0d1183d5ed7c1),
     C(32264b75)},
    {C(de9e0cb0e16f6e6d), C(238e6283aa4f6594), C(4fb9c914c2f0a13b),
     C(a64b3376)},
    {C(6d4b876d9b146d1a), C(aab2d64ce8f26739), C(d315f93600e83fe5), C(d33890e)},
    {C(e698fa3f54e6ea22), C(bd28e20e7455358c), C(9ace161f6ea76e66),
     C(926d4b63)},
    {C(7bc0deed4fb349f7), C(1771aff25dc722fa), C(19ff0644d9681917),
     C(d51ba539)},
    {C(db4b15e88533f622), C(256d6d2419b41ce9), C(9d7c5378396765d5),
     C(7f37636d)},
    {C(922834735e86ecb2), C(363382685b88328e), C(e9c92960d7144630),
     C(b98026c0)},
    {C(30f1d72c812f1eb8), C(b567cd4a69cd8989), C(820b6c992a51f0bc),
     C(b877767e)},
    {C(168884267f3817e9), C(5b376e050f637645), C(1c18314abd34497a), C(aefae77)},
    {C(82e78596ee3e56a7), C(25697d9c87f30d98), C(7600a8342834924d), C(f686911)},
    {C(aa2d6cf22e3cc252), C(9b4dec4f5e179f16), C(76fb0fba1d99a99a),
     C(3deadf12)},
    {C(7bf5ffd7f69385c7), C(fc077b1d8bc82879), C(9c04e36f9ed83a24),
     C(ccf02a4e)},
    {C(e89c8ff9f9c6e34b), C(f54c0f669a49f6c4), C(fc3e46f5d846adef),
     C(176c1722)},
    {C(a18fbcdccd11e1f4), C(8248216751dfd65e), C(40c089f208d89d7c), C(26f82ad)},
    {C(2d54f40cc4088b17), C(59d15633b0cd1399), C(a8cc04bb1bffd15b),
     C(b5244f42)},
    {C(69276946cb4e87c7), C(62bdbe6183be6fa9), C(3ba9773dac442a1a),
     C(49a689e5)},
    {C(668174a3f443df1d), C(407299392da1ce86), C(c2a3f7d7f2c5be28), C(59fcdd3)},
    {C(5e29be847bd5046), C(b561c7f19c8f80c3), C(5e5abd5021ccaeaf), C(4f4b04e9)},
    {C(cd0d79f2164da014), C(4c386bb5c5d6ca0c), C(8e771b03647c3b63),
     C(8b00f891)},
    {C(e0e6fc0b1628af1d), C(29be5fb4c27a2949), C(1c3f781a604d3630),
     C(16e114f3)},
    {C(2058927664adfd93), C(6e8f968c7963baa5), C(af3dced6fff7c394),
     C(d6b6dadc)},
    {C(dc107285fd8e1af7), C(a8641a0609321f3f), C(db06e89ffdc54466),
     C(897e20ac)},
    {C(fbba1afe2e3280f1), C(755a5f392f07fce), C(9e44a9a15402809a), C(f996e05d)},
    {C(bfa10785ddc1011b), C(b6e1c4d2f670f7de), C(517d95604e4fcc1f),
     C(c4306af6)},
    {C(534cc35f0ee1eb4e), C(b703820f1f3b3dce), C(884aa164cf22363), C(6dcad433)},
    {C(7ca6e3933995dac), C(fd118c77daa8188), C(3aceb7b5e7da6545), C(3c07374d)},
    {C(f0d6044f6efd7598), C(e044d6ba4369856e), C(91968e4f8c8a1a4c),
     C(f0f4602c)},
    {C(3d69e52049879d61), C(76610636ea9f74fe), C(e9bf5602f89310c0),
     C(3e1ea071)},
    {C(79da242a16acae31), C(183c5f438e29d40), C(6d351710ae92f3de), C(67580f0c)},
    {C(461c82656a74fb57), C(d84b491b275aa0f7), C(8f262cb29a6eb8b2),
     C(4e109454)},
    {C(53c1a66d0b13003), C(731f060e6fe797fc), C(daa56811791371e3), C(88a474a7)},
    {C(d3a2efec0f047e9), C(1cabce58853e58ea), C(7a17b2eae3256be4), C(5b5bedd)},
    {C(43c64d7484f7f9b2), C(5da002b64aafaeb7), C(b576c1e45800a716),
     C(1aaddfa7)},
    {C(a7dec6ad81cf7fa1), C(180c1ab708683063), C(95e0fd7008d67cff),
     C(5be07fd8)},
    {C(5408a1df99d4aff), C(b9565e588740f6bd), C(abf241813b08006e), C(cbca8606)},
    {C(a8b27a6bcaeeed4b), C(aec1eeded6a87e39), C(9daf246d6fed8326),
     C(bde64d01)},
    {C(9a952a8246fdc269), C(d0dcfcac74ef278c), C(250f7139836f0f1f),
     C(ee90cf33)},
    {C(c930841d1d88684f), C(5eb66eb18b7f9672), C(e455d413008a2546),
     C(4305c3ce)},
    {C(94dc6971e3cf071a), C(994c7003b73b2b34), C(ea16e85978694e5), C(4b3a1d76)},
    {C(7fc98006e25cac9), C(77fee0484cda86a7), C(376ec3d447060456), C(a8bb6d80)},
    {C(bd781c4454103f6), C(612197322f49c931), C(b9cf17fd7e5462d5), C(1f9fa607)},
    {C(da60e6b14479f9df), C(3bdccf69ece16792), C(18ebf45c4fecfdc9),
     C(8d0e4ed2)},
    {C(4ca56a348b6c4d3), C(60618537c3872514), C(2fbb9f0e65871b09), C(1bf31347)},
    {C(ebd22d4b70946401), C(6863602bf7139017), C(c0b1ac4e11b00666),
     C(1ae3fc5b)},
    {C(3cc4693d6cbcb0c), C(501689ea1c70ffa), C(10a4353e9c89e364), C(459c3930)},
    {C(38908e43f7ba5ef0), C(1ab035d4e7781e76), C(41d133e8c0a68ff7),
     C(e00c4184)},
    {C(34983ccc6aa40205), C(21802cad34e72bc4), C(1943e8fb3c17bb8), C(ffc7a781)},
    {C(86215c45dcac9905), C(ea546afe851cae4b), C(d85b6457e489e374),
     C(6a125480)},
    {C(420fc255c38db175), C(d503cd0f3c1208d1), C(d4684e74c825a0bc),
     C(88a1512b)},
    {C(1d7a31f5bc8fe2f9), C(4763991092dcf836), C(ed695f55b97416f4),
     C(549bbbe5)},
    {C(94129a84c376a26e), C(c245e859dc231933), C(1b8f74fecf917453),
     C(c133d38c)},
    {C(1d3a9809dab05c8d), C(adddeb4f71c93e8), C(ef342eb36631edb), C(fcace348)},
    {C(90fa3ccbd60848da), C(dfa6e0595b569e11), C(e585d067a1f5135d),
     C(ed7b6f9a)},
    {C(2dbb4fc71b554514), C(9650e04b86be0f82), C(60f2304fba9274d3),
     C(6d907dda)},
    {C(b98bf4274d18374a), C(1b669fd4c7f9a19a), C(b1f5972b88ba2b7a),
     C(7a4d48d5)},
    {C(d6781d0b5e18eb68), C(b992913cae09b533), C(58f6021caaee3a40),
     C(e686f3db)},
    {C(226651cf18f4884c), C(595052a874f0f51c), C(c9b75162b23bab42), C(cce7c55)},
    {C(a734fb047d3162d6), C(e523170d240ba3a5), C(125a6972809730e8), C(f58b96b)},
    {C(c6df6364a24f75a3), C(c294e2c84c4f5df8), C(a88df65c6a89313b),
     C(1bbf6f60)},
    {C(d8d1364c1fbcd10), C(2d7cc7f54832deaa), C(4e22c876a7c57625), C(ce5e0cc2)},
    {C(aae06f9146db885f), C(3598736441e280d9), C(fba339b117083e55),
     C(584cfd6f)},
    {C(8955ef07631e3bcc), C(7d70965ea3926f83), C(39aed4134f8b2db6),
     C(8f9bbc33)},
    {C(ad611c609cfbe412), C(d3c00b18bf253877), C(90b2172e1f3d0bfd),
     C(d7640d95)},
    {C(d5339adc295d5d69), C(b633cc1dcb8b586a), C(ee84184cf5b1aeaf), C(3d12a2b)},
    {C(40d0aeff521375a8), C(77ba1ad7ecebd506), C(547c6f1a7d9df427),
     C(aaeafed0)},
    {C(8b2d54ae1a3df769), C(11e7adaee3216679), C(3483781efc563e03),
     C(95b9b814)},
    {C(99c175819b4eae28), C(932e8ff9f7a40043), C(ec78dcab07ca9f7c),
     C(45fbe66e)},
    {C(2a418335779b82fc), C(af0295987849a76b), C(c12bc5ff0213f46e),
     C(b4baa7a8)},
    {C(3b1fc6a3d279e67d), C(70ea1e49c226396), C(25505adcf104697c), C(83e962fe)},
    {C(d97eacdf10f1c3c9), C(b54f4654043a36e0), C(b128f6eb09d1234), C(aac3531c)},
    {C(293a5c1c4e203cd4), C(6b3329f1c130cefe), C(f2e32f8ec76aac91),
     C(2b1db7cc)},
    {C(4290e018ffaedde7), C(a14948545418eb5e), C(72d851b202284636),
     C(cf00cd31)},
    {C(f919a59cbde8bf2f), C(a56d04203b2dc5a5), C(38b06753ac871e48),
     C(7d3c43b8)},
    {C(1d70a3f5521d7fa4), C(fb97b3fdc5891965), C(299d49bbbe3535af),
     C(cbd5fac6)},
    {C(6af98d7b656d0d7c), C(d2e99ae96d6b5c0c), C(f63bd1603ef80627),
     C(76d0fec4)},
    {C(395b7a8adb96ab75), C(582df7165b20f4a), C(e52bd30e9ff657f9), C(405e3402)},
    {C(3822dd82c7df012f), C(b9029b40bd9f122b), C(fd25b988468266c4),
     C(c732c481)},
    {C(79f7efe4a80b951a), C(dd3a3fddfc6c9c41), C(ab4c812f9e27aa40),
     C(a8d123c9)},
    {C(ae6e59f5f055921a), C(e9d9b7bf68e82), C(5ce4e4a5b269cc59), C(1e80ad7d)},
    {C(8959dbbf07387d36), C(b4658afce48ea35d), C(8f3f82437d8cb8d6),
     C(52aeb863)},
    {C(4739613234278a49), C(99ea5bcd340bf663), C(258640912e712b12),
     C(ef7c0c18)},
    {C(420e6c926bc54841), C(96dbbf6f4e7c75cd), C(d8d40fa70c3c67bb),
     C(b6ad4b68)},
    {C(c8601bab561bc1b7), C(72b26272a0ff869a), C(56fdfc986d6bc3c4),
     C(c1e46b17)},
    {C(b2d294931a0e20eb), C(284ffd9a0815bc38), C(1f8a103aac9bbe6), C(57b8df25)},
    {C(7966f53c37b6c6d7), C(8e6abcfb3aa2b88f), C(7f2e5e0724e5f345),
     C(e9fa36d6)},
    {C(be9bb0abd03b7368), C(13bca93a3031be55), C(e864f4f52b55b472),
     C(8f8daefc)},
    {C(a08d128c5f1649be), C(a8166c3dbbe19aad), C(cb9f914f829ec62c), C(6e1bb7e)},
    {C(7c386f0ffe0465ac), C(530419c9d843dbf3), C(7450e3a4f72b8d8c),
     C(fd0076f0)},
    {C(bb362094e7ef4f8), C(ff3c2a48966f9725), C(55152803acd4a7fe), C(899b17b6)},
    {C(cd80dea24321eea4), C(52b4fdc8130c2b15), C(f3ea100b154bfb82),
     C(e3e84e31)},
    {C(d599a04125372c3a), C(313136c56a56f363), C(1e993c3677625832),
     C(eef79b6b)},
    {C(dbbf541e9dfda0a), C(1479fceb6db4f844), C(31ab576b59062534), C(868e3315)},
    {C(c2ee3288be4fe2bf), C(c65d2f5ddf32b92), C(af6ecdf121ba5485), C(4639a426)},
    {C(d86603ced1ed4730), C(f9de718aaada7709), C(db8b9755194c6535),
     C(f3213646)},
    {C(915263c671b28809), C(a815378e7ad762fd), C(abec6dc9b669f559),
     C(17f148e9)},
    {C(2b67cdd38c307a5e), C(cb1d45bb5c9fe1c), C(800baf2a02ec18ad), C(bfd94880)},
    {C(2d107419073b9cd0), C(a96db0740cef8f54), C(ec41ee91b3ecdc1b),
     C(bb1fa7f3)},
    {C(f3e9487ec0e26dfc), C(1ab1f63224e837fa), C(119983bb5a8125d8), C(88816b1)},
    {C(1160987c8fe86f7d), C(879e6db1481eb91b), C(d7dcb802bfe6885d),
     C(5c2faeb3)},
    {C(eab8112c560b967b), C(97f550b58e89dbae), C(846ed506d304051f),
     C(51b5fc6f)},
    {C(1addcf0386d35351), C(b5f436561f8f1484), C(85d38e22181c9bb1),
     C(33d94752)},
    {C(d445ba84bf803e09), C(1216c2497038f804), C(2293216ea2237207),
     C(b0c92948)},
    {C(37235a096a8be435), C(d9b73130493589c2), C(3b1024f59378d3be),
     C(c7171590)},
    {C(763ad6ea2fe1c99d), C(cf7af5368ac1e26b), C(4d5e451b3bb8d3d4),
     C(240a67fb)},
    {C(ea627fc84cd1b857), C(85e372494520071f), C(69ec61800845780b),
     C(e1843cd5)},
    {C(1f2ffd79f2cdc0c8), C(726a1bc31b337aaa), C(678b7f275ef96434),
     C(fda1452b)},
    {C(39a9e146ec4b3210), C(f63f75802a78b1ac), C(e2e22539c94741c3),
     C(a2cad330)},
    {C(74cba303e2dd9d6d), C(692699b83289fad1), C(dfb9aa7874678480),
     C(53467e16)},
    {C(4cbc2b73a43071e0), C(56c5db4c4ca4e0b7), C(1b275a162f46bd3d),
     C(da14a8d0)},
    {C(875638b9715d2221), C(d9ba0615c0c58740), C(616d4be2dfe825aa),
     C(67333551)},
    {C(fb686b2782994a8d), C(edee60693756bb48), C(e6bc3cae0ded2ef5),
     C(a0ebd66e)},
    {C(ab21d81a911e6723), C(4c31b07354852f59), C(835da384c9384744),
     C(4b769593)},
    {C(33d013cc0cd46ecf), C(3de726423aea122c), C(116af51117fe21a9),
     C(6aa75624)},
    {C(8ca92c7cd39fae5d), C(317e620e1bf20f1), C(4f0b33bf2194b97f), C(602a3f96)},
    {C(fdde3b03f018f43e), C(38f932946c78660), C(c84084ce946851ee), C(cd183c4d)},
    {C(9c8502050e9c9458), C(d6d2a1a69964beb9), C(1675766f480229b5),
     C(960a4d07)},
    {C(348176ca2fa2fdd2), C(3a89c514cc360c2d), C(9f90b8afb318d6d0),
     C(9ae998c4)},
    {C(4a3d3dfbbaea130b), C(4e221c920f61ed01), C(553fd6cd1304531f),
     C(74e2179d)},
    {C(b371f768cdf4edb9), C(bdef2ace6d2de0f0), C(e05b4100f7f1baec),
     C(ee9bae25)},
    {C(7a1d2e96934f61f), C(eb1760ae6af7d961), C(887eb0da063005df), C(b66edf10)},
    {C(8be53d466d4728f2), C(86a5ac8e0d416640), C(984aa464cdb5c8bb),
     C(d6209737)},
    {C(829677eb03abf042), C(43cad004b6bc2c0), C(f2f224756803971a), C(b994a88)},
    {C(754435bae3496fc), C(5707fc006f094dcf), C(8951c86ab19d8e40), C(a05d43c0)},
    {C(fda9877ea8e3805f), C(31e868b6ffd521b7), C(b08c90681fb6a0fd),
     C(c79f73a8)},
    {C(2e36f523ca8f5eb5), C(8b22932f89b27513), C(331cd6ecbfadc1bb),
     C(a490aff5)},
    {C(21a378ef76828208), C(a5c13037fa841da2), C(506d22a53fbe9812),
     C(dfad65b4)},
    {C(ccdd5600054b16ca), C(f78846e84204cb7b), C(1f9faec82c24eac9), C(1d07dfb)},
    {C(7854468f4e0cabd0), C(3a3f6b4f098d0692), C(ae2423ec7799d30d),
     C(416df9a0)},
    {C(7f88db5346d8f997), C(88eac9aacc653798), C(68a4d0295f8eefa1),
     C(1f8fb9cc)},
    {C(bb3fb5fb01d60fcf), C(1b7cc0847a215eb6), C(1246c994437990a1),
     C(7abf48e3)},
    {C(2e783e1761acd84d), C(39158042bac975a0), C(1cd21c5a8071188d),
     C(dea4e3dd)},
    {C(392058251cf22acc), C(944ec4475ead4620), C(b330a10b5cb94166),
     C(c6064f22)},
    {C(adf5c1e5d6419947), C(2a9747bc659d28aa), C(95c5b8cb1f5d62c), C(743bed9c)},
    {C(6bc1db2c2bee5aba), C(e63b0ed635307398), C(7b2eca111f30dbbc),
     C(fce254d5)},
    {C(b00f898229efa508), C(83b7590ad7f6985c), C(2780e70a0592e41d),
     C(e47ec9d1)},
    {C(b56eb769ce0d9a8c), C(ce196117bfbcaf04), C(b26c3c3797d66165),
     C(334a145c)},
    {C(70c0637675b94150), C(259e1669305b0a15), C(46e1dd9fd387a58d),
     C(adec1e3c)},
    {C(74c0b8a6821faafe), C(abac39d7491370e7), C(faf0b2a48a4e6aed),
     C(f6a9fbf8)},
    {C(5fb5e48ac7b7fa4f), C(a96170f08f5acbc7), C(bbf5c63d4f52a1e5),
     C(5398210c)},
};

void TestUnchanging(const uint64_t* expected, int offset, int len) {
  EXPECT_EQ(expected[0], CityHash64(data + offset, len));
  EXPECT_EQ(expected[3], CityHash32(data + offset, len));
  EXPECT_EQ(expected[1], CityHash64WithSeed(data + offset, len, kSeed0));
  EXPECT_EQ(expected[2],
            CityHash64WithSeeds(data + offset, len, kSeed0, kSeed1));
}

TEST(CityHashTest, Unchanging) {
  setup();
  int i = 0;
  for (; i < kTestSize - 1; i++) {
    TestUnchanging(testdata[i], i * i, i);
  }
  TestUnchanging(testdata[i], 0, kDataSize);
}

}  // namespace hash_internal
}  // namespace basic
