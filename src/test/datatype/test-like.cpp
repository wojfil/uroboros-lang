/*
    This file is part of Perun2.
    Perun2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Peruns2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Perun2. If not, see <http://www.gnu.org/licenses/>.
*/

#include "test.h"
#include "test-like.h"
#include "../../datatype/generator/gen-like.h"
#include <iostream>

using namespace perun2;

void test_like()
{
   testCase_like(1, L"u%", L"uro", true);
   testCase_like(2, L"b%", L"uro", false);
   testCase_like(3, L"%", L"", true);
   testCase_like(4, L"%", L" L", true);
   testCase_like(5, L"%", L"asdfa asdf asdf", true);
   testCase_like(6, L"%", L"%", true);
   testCase_like(7, L"_", L"", false);
   testCase_like(8, L"_", L" ", true);
   testCase_like(9, L"_", L"4", true);
   testCase_like(10, L"_", L"C", true);
   testCase_like(11, L"_", L"CX", false);
   testCase_like(12, L"[ABCD]", L"", false);
   testCase_like(13, L"[ABCD]", L"A", true);
   testCase_like(14, L"[ABCD]", L"b", false);
   testCase_like(15, L"[ABCD]", L"X", false);
   testCase_like(16, L"[ABCD]", L"AB", false);
   testCase_like(17, L"[B-D]", L"C", true);
   testCase_like(18, L"[B-D]", L"D", true);
   testCase_like(19, L"[B-D]", L"A", false);
   testCase_like(20, L"[^B-D]", L"C", false);
   testCase_like(21, L"[^B-D]", L"D", false);
   testCase_like(22, L"[^B-D]", L"A", true);
   testCase_like(23, L"%TEST[ABCD]XXX", L"lolTESTBXXX", true);
   testCase_like(24, L"%TEST[ABCD]XXX", L"lolTESTZXXX", false);
   testCase_like(25, L"%TEST[^ABCD]XXX", L"lolTESTBXXX", false);
   testCase_like(26, L"%TEST[^ABCD]XXX", L"lolTESTZXXX", true);
   testCase_like(27, L"%TEST[B-D]XXX", L"lolTESTBXXX", true);
   testCase_like(28, L"%TEST[^B-D]XXX", L"lolTESTZXXX", true);
   testCase_like(29, L"%Uroboros.txt", L"Uroboros.txt", true);
   testCase_like(30, L"%Uroboros.txt", L"TestUroboros.txt", true);
   testCase_like(31, L"%Uroboros.txt", L"TestUroboros.txt.img", false);
   testCase_like(32, L"%Uroboros.txt", L"Uroboros.txt.img", false);
   testCase_like(33, L"%Uroboros.txt", L"TestUroboros001.txt.img", false);
   testCase_like(34, L"Uroboros.txt%", L"Uroboros.txt", true);
   testCase_like(35, L"Uroboros.txt%", L"TestUroboros.txt", false);
   testCase_like(36, L"Uroboros.txt%", L"TestUroboros.txt.img", false);
   testCase_like(37, L"Uroboros.txt%", L"Uroboros.txt.img", true);
   testCase_like(38, L"Uroboros.txt%", L"TestUroboros001.txt.img", false);
   testCase_like(39, L"%Uroboros.txt%", L"Uroboros.txt", true);
   testCase_like(40, L"%Uroboros.txt%", L"TestUroboros.txt", true);
   testCase_like(41, L"%Uroboros.txt%", L"TestUroboros.txt.img", true);
   testCase_like(42, L"%Uroboros.txt%", L"Uroboros.txt.img", true);
   testCase_like(43, L"%Uroboros.txt%", L"TestUroboros001.txt.img", false);
   testCase_like(44, L"%Uroboros%.txt", L"Uroboros.txt", true);
   testCase_like(45, L"%Uroboros%.txt", L"TestUroboros.txt", true);
   testCase_like(46, L"%Uroboros%.txt", L"TestUroboros.txt.img", false);
   testCase_like(47, L"%Uroboros%.txt", L"Uroboros.txt.img", false);
   testCase_like(48, L"%Uroboros%.txt", L"TestUroboros001.txt.img", false);
   testCase_like(49, L"%Uroboros%.txt", L"TestUroboros001.txt", true);
   testCase_like(50, L"Uroboros%.txt%", L"Uroboros.txt", true);
   testCase_like(51, L"Uroboros%.txt%", L"TestUroboros.txt", false);
   testCase_like(52, L"Uroboros%.txt%", L"TestUroboros.txt.img", false);
   testCase_like(53, L"Uroboros%.txt%", L"Uroboros.txt.img", true);
   testCase_like(54, L"Uroboros%.txt%", L"TestUroboros001.txt.img", false);
   testCase_like(55, L"Uroboros%.txt%", L"TestUroboros001.txt", false);
   testCase_like(56, L"%Uroboros%.txt%", L"Uroboros.txt", true);
   testCase_like(57, L"%Uroboros%.txt%", L"TestUroboros.txt", true);
   testCase_like(58, L"%Uroboros%.txt%", L"TestUroboros.txt.img", true);
   testCase_like(59, L"%Uroboros%.txt%", L"Uroboros.txt.img", true);
   testCase_like(60, L"%Uroboros%.txt%", L"TestUroboros001.txt.img", true);
   testCase_like(61, L"%Uroboros%.txt%", L"TestUroboros001.txt", true);
   testCase_like(62, L"_Uroboros_.txt_", L"1Uroboros3.txt4", true);
   testCase_like(63, L"_Uroboros_.txt_", L"1Uroboros.txt4", false);
   testCase_like(64, L"_Uroboros_.txt_", L"1Uroboros3.txt", false);
   testCase_like(65, L"_Uroboros_.txt_", L"Uroboros3.txt4", false);
   testCase_like(66, L"____", L"abdc", true);
   testCase_like(67, L"____", L"abc", false);
   testCase_like(68, L"___#", L"abcd", false);
   testCase_like(69, L"___#", L"abc5", true);
   testCase_like(70, L"%#%", L"Uroboros", false);
   testCase_like(71, L"%#%", L"1Uroboros", true);
   testCase_like(72, L"%#%", L"Uroboros3", true);
   testCase_like(73, L"%#%", L"Urob7oros", true);
   testCase_like(74, L"#_#", L"3aa", false);
   testCase_like(75, L"#_#", L"320", true);
   testCase_like(76, L"#%", L"320", true);
   testCase_like(77, L"##", L"320", false);
   testCase_like(78, L"[0-9]#", L"32", true);
   testCase_like(79, L"[0-9]#", L"a2", false);
   testCase_like(80, L"ur#b#r#s", L"ur0b0r0s", true);
   testCase_like(81, L"ur#b#r#s", L"ur0b0ros", false);
   testCase_like(82, L"[0-9]g", L"a2", false);
   testCase_like(83, L"%opo%", L"Popol Vuh", true);
   testCase_like(84, L"[^^]%", L"something", true);
   testCase_like(85, L"[^^]%", L"  ", true);
   testCase_like(86, L"[^^]%", L"", false);
   testCase_like(87, L"[^^]%", L"aa", true);
   testCase_like(88, L"[^^]%", L"%", true);
   testCase_like(89, L"[^^]%", L"^", false);
   testCase_like(90, L"[^]%", L"another day", true);
   testCase_like(91, L"[^]%", L"  ", true);
   testCase_like(92, L"[^]%", L"", false);
   testCase_like(93, L"[^]%", L"aa", true);
   testCase_like(94, L"[^]%", L"%", true);
   testCase_like(95, L"[^]%", L"^", true);
   testCase_like(96, L"[^]_", L"a.txt", false);
   testCase_like(97, L"[^]_", L"  ", true);
   testCase_like(98, L"[^]_", L"", false);
   testCase_like(99, L"[^]_", L"aa", true);
   testCase_like(100, L"[^]_", L"%", false);
   testCase_like(101, L"[^]_", L"^", false);
   testCase_like(102, L"_[^]_", L"water?", false);
   testCase_like(103, L"_[^]_", L" g ", true);
   testCase_like(104, L"_[^]_", L"", false);
   testCase_like(105, L"_[^]_", L"aaa", true);
   testCase_like(106, L"_[^]_", L"%6", false);
   testCase_like(107, L"_[^]_", L"^", false);
   testCase_like(108, L"[]%", L"fence", false);
   testCase_like(109, L"[]%", L" g ", false);
   testCase_like(110, L"[]%", L"", false);
   testCase_like(111, L"[]%", L"aaa", false);
   testCase_like(112, L"[]%", L"%6", false);
   testCase_like(113, L"[]%", L"^", false);
   testCase_like(114, L"[-]%", L"patt", false);
   testCase_like(115, L"[-]%", L" g ", false);
   testCase_like(116, L"[-]%", L"", false);
   testCase_like(117, L"[-]%", L"-aaa", true);
   testCase_like(118, L"[-]%", L"U", false);
   testCase_like(119, L"[-]%", L"%6", false);
   testCase_like(120, L"[-]%", L"-", true);
   testCase_like(121, L"[-U]%", L"word", false);
   testCase_like(122, L"[-U]%", L" g ", false);
   testCase_like(123, L"[-U]%", L"", false);
   testCase_like(124, L"[-U]%", L"-aaa", true);
   testCase_like(125, L"[-U]%", L"U", true);
   testCase_like(126, L"[-U]%", L"%6", false);
   testCase_like(127, L"[-U]%", L"-", true);
   testCase_like(128, L"[-Uw]%", L"ant", false);
   testCase_like(129, L"[-Uw]%", L" g ", false);
   testCase_like(130, L"[-Uw]%", L"", false);
   testCase_like(131, L"[-Uw]%", L"worm", true);
   testCase_like(132, L"[-Uw]%", L"Worms", false);
   testCase_like(133, L"[-Uw]%", L"-aaa", true);
   testCase_like(134, L"[-Uw]%", L"U", true);
   testCase_like(135, L"[-Uw]%", L"%6", false);
   testCase_like(136, L"[-Uw]%", L"-", true);
   testCase_like(137, L"_[b-d-]", L"ant", false);
   testCase_like(138, L"_[b-d-]", L"an", false);
   testCase_like(139, L"_[b-d-]", L"b-", true);
   testCase_like(140, L"_[b-d-]", L"--", true);
   testCase_like(141, L"_[b-d-]", L"-c", true);
   testCase_like(142, L"_[b-d-]", L"-e", false);
   testCase_like(143, L"_[b-d-]", L"-eh", false);
   testCase_like(144, L"_[b-d-]", L"accurate", false);
   testCase_like(145, L"_[b-d-]", L"-", false);
   testCase_like(146, L"_[b-d-]", L"", false);
   testCase_like(147, L"[e-g][s-u]", L"ft", true);
   testCase_like(148, L"[e-g][s-u]", L"fT", false);
   testCase_like(149, L"[e-g][s-u]", L"at", false);
   testCase_like(150, L"[e-g][s-u]", L"tf", false);
   testCase_like(151, L"[e-g][u-s]", L"ft", true);
   testCase_like(152, L"[e-g][s-u]", L"ess", false);
   testCase_like(153, L"[e-g][s-u]", L"es", true);
   testCase_like(154, L"[e-g][s-u]", L"- ", false);
   testCase_like(155, L"[e-g][s-u]", L"  ", false);
   testCase_like(156, L"[e-g][s-u]", L"", false);
   testCase_like(157, L"[e-gs-u]%", L"feel", true);
   testCase_like(158, L"[e-gs-u]%", L" feel", false);
   testCase_like(159, L"[e-gs-u]%", L"things", true);
   testCase_like(160, L"[e-gs-u]%", L"uroboros", true);
   testCase_like(161, L"[e-gs-u]%", L"vents", false);
   testCase_like(162, L"[e-gs-u]%", L"u", true);
   testCase_like(163, L"[e-gs-u]%", L"-", false);
   testCase_like(164, L"[e-gs-u]%", L"--", false);
   testCase_like(165, L"[e-gs-u]%", L"z", false);
   testCase_like(166, L"[e-gs-u]%", L"", false);
   testCase_like(167, L"[e-gs-u]%", L"alone", false);
   testCase_like(168, L"[e-gs-u]%", L"faces", true);
   testCase_like(169, L"[e-gs-u]%", L"-faces", false);
   testCase_like(170, L"[e-gs-u]%", L"early", true);
   testCase_like(171, L"[e-gs-u]%", L"history", false);
   testCase_like(172, L"[e-gs-u]%", L"raven", false);
   testCase_like(173, L"[e-gs-u]%", L"r", false);
   testCase_like(174, L"[e-gs-u]%", L"steal", true);
   testCase_like(175, L"[e-gs-u]%", L"traits", true);
   testCase_like(176, L"[e-gs-u]%", L" traits", false);
   testCase_like(177, L"[e-gs-u]%", L"-steal", false);
   testCase_like(178, L"[e-gs-u]%", L"%steal", false);
   testCase_like(179, L"[e-gs-u]%[e-gs-u]", L"feel", false);
   testCase_like(180, L"[e-gs-u]%[e-gs-u]", L" feel", false);
   testCase_like(181, L"[e-gs-u]%[e-gs-u]", L"things", true);
   testCase_like(182, L"[e-gs-u]%[e-gs-u]", L"uroboros", true);
   testCase_like(183, L"[e-gs-u]%[e-gs-u]", L"vents", false);
   testCase_like(184, L"[e-gs-u]%[e-gs-u]", L"u", false);
   testCase_like(185, L"[e-gs-u]%[e-gs-u]", L"-", false);
   testCase_like(186, L"[e-gs-u]%[e-gs-u]", L"--", false);
   testCase_like(187, L"[e-gs-u]%[e-gs-u]", L"z", false);
   testCase_like(188, L"[e-gs-u]%[e-gs-u]", L"", false);
   testCase_like(189, L"[e-gs-u]%[e-gs-u]", L"alone", false);
   testCase_like(190, L"[e-gs-u]%[e-gs-u]", L"faces", true);
   testCase_like(191, L"[e-gs-u]%[e-gs-u]", L"-faces", false);
   testCase_like(192, L"[e-gs-u]%[e-gs-u]", L"early", false);
   testCase_like(193, L"[e-gs-u]%[e-gs-u]", L"historias", false);
   testCase_like(194, L"[e-gs-u]%[e-gs-u]", L"raven", false);
   testCase_like(195, L"[e-gs-u]%[e-gs-u]", L"r", false);
   testCase_like(196, L"[e-gs-u]%[e-gs-u]", L"stealf", true);
   testCase_like(197, L"[e-gs-u]%[e-gs-u]", L"traits", true);
   testCase_like(198, L"[e-gs-u]%[e-gs-u]", L" traits", false);
   testCase_like(199, L"[e-gs-u]%[e-gs-u]", L"-steal", false);
   testCase_like(200, L"[e-gs-u]%[e-gs-u]", L"%steal", false);
   testCase_like(201, L"[e-gs-u-]%", L"feel", true);
   testCase_like(202, L"[e-gs-u-]%", L" feel", false);
   testCase_like(203, L"[e-gs-u-]%", L"things", true);
   testCase_like(204, L"[e-gs-u-]%", L"uroboros", true);
   testCase_like(205, L"[e-gs-u-]%", L"vents", false);
   testCase_like(206, L"[e-gs-u-]%", L"u", true);
   testCase_like(207, L"[e-gs-u-]%", L"-", true);
   testCase_like(208, L"[e-gs-u-]%", L"--", true);
   testCase_like(209, L"[e-gs-u-]%", L"z", false);
   testCase_like(210, L"[e-gs-u-]%", L"", false);
   testCase_like(211, L"[e-gs-u-]%", L"alone", false);
   testCase_like(212, L"[e-gs-u-]%", L"faces", true);
   testCase_like(213, L"[e-gs-u-]%", L"-faces", true);
   testCase_like(214, L"[e-gs-u-]%", L"early", true);
   testCase_like(215, L"[e-gs-u-]%", L"history", false);
   testCase_like(216, L"[e-gs-u-]%", L"raven", false);
   testCase_like(217, L"[e-gs-u-]%", L"r", false);
   testCase_like(218, L"[e-gs-u-]%", L"steal", true);
   testCase_like(219, L"[e-gs-u-]%", L"traits", true);
   testCase_like(220, L"[e-gs-u-]%", L" traits", false);
   testCase_like(221, L"[e-gs-u-]%", L"-steal", true);
   testCase_like(222, L"[e-gs-u-]%", L"%steal", false);
   testCase_like(223, L"[-e-g]%", L"feel", true);
   testCase_like(224, L"[-e-g]%", L" feel", false);
   testCase_like(225, L"[-e-g]%", L"things", false);
   testCase_like(226, L"[-e-g]%", L"uroboros", false);
   testCase_like(227, L"[-e-g]%", L"vents", false);
   testCase_like(228, L"[-e-g]%", L"u", false);
   testCase_like(229, L"[-e-g]%", L"-", true);
   testCase_like(230, L"[-e-g]%", L"--", true);
   testCase_like(231, L"[-e-g]%", L"z", false);
   testCase_like(232, L"[-e-g]%", L"", false);
   testCase_like(233, L"[-e-g]%", L"alone", false);
   testCase_like(234, L"[-e-g]%", L"faces", true);
   testCase_like(235, L"[-e-g]%", L"-faces", true);
   testCase_like(236, L"[-e-g]%", L"early", true);
   testCase_like(237, L"[-e-g]%", L"historias", false);
   testCase_like(238, L"[-e-g]%", L"raven", false);
   testCase_like(239, L"[-e-g]%", L"r", false);
   testCase_like(240, L"[-e-g]%", L"stealthy", false);
   testCase_like(241, L"[-e-g]%", L"traits", false);
   testCase_like(242, L"[-e-g]%", L" traits", false);
   testCase_like(243, L"[-e-g]%", L"-steal", true);
   testCase_like(244, L"[-e-g]%", L"%steal", false);
   testCase_like(245, L"#[a^c]", L"3a", true);
   testCase_like(246, L"#[a^c]", L"aa", false);
   testCase_like(247, L"#[a^c]", L"6c", true);
   testCase_like(248, L"#[a^c]", L"2b", false);
   testCase_like(249, L"#[a^c]", L"2 ", false);
   testCase_like(250, L"#[a^c]", L"", false);
   testCase_like(251, L"#[a^c]", L"0c ", false);
   testCase_like(252, L"#[a^c]", L"00", false);
   testCase_like(253, L"#[a^c]", L"0^", true);
   testCase_like(254, L"#[a^c]", L"^^", false);
   testCase_like(255, L"#[a^c]", L" ^", false);
   testCase_like(256, L"#[a^c]", L"7^", true);
   testCase_like(257, L"[^ab]#", L"a6", false);
   testCase_like(258, L"[^ab]#", L"z6", true);
   testCase_like(259, L"[^ab]#", L"c0", true);
   testCase_like(260, L"[^ab]#", L"cc", false);
   testCase_like(261, L"[^ab]#", L"c99", false);
   testCase_like(262, L"[^ab]#", L"c ", false);
   testCase_like(263, L"[^ab]#", L"b6", false);
   testCase_like(264, L"[^ab]#", L"u65", false);
   testCase_like(265, L"[^ab]#", L"u6", true);
   testCase_like(266, L"[a^b]", L"a", true);
   testCase_like(267, L"[a^b]", L"b", true);
   testCase_like(268, L"[a^b]", L"^", true);
   testCase_like(269, L"[a^b]", L" ", false);
   testCase_like(270, L"[a^b]", L"?", false);
   testCase_like(271, L"[a^b]", L"c", false);
   testCase_like(272, L"[a^b]", L"", false);
   testCase_like(273, L"[a^b]", L"5", false);
   testCase_like(274, L"[a^b]", L"aa", false);
   testCase_like(275, L"[ab^]", L"a", true);
   testCase_like(276, L"[ab^]", L"b", true);
   testCase_like(277, L"[ab^]", L"^", true);
   testCase_like(278, L"[ab^]", L" ", false);
   testCase_like(279, L"[ab^]", L"?", false);
   testCase_like(280, L"[ab^]", L"c", false);
   testCase_like(281, L"[ab^]", L"", false);
   testCase_like(282, L"[ab^]", L"5", false);
   testCase_like(283, L"[ab^]", L"aa", false);
   testCase_like(284, L"ta[[]##]%", L"ta[21]", true);
   testCase_like(285, L"ta[[]##]%", L"ta[11]", true);
   testCase_like(286, L"ta[[]##]%", L"ta[0]", false);
   testCase_like(287, L"ta[[]##]%", L"ta[]", false);
   testCase_like(288, L"ta[[]##]%", L"ta[11]ki", true);
   testCase_like(289, L"ta[[]##]%", L"ta[114]ki", false);
   testCase_like(290, L"ta[[]##]%", L"ta]11]ki", false);
   testCase_like(291, L"ta[[]##]%", L"ta[21", false);
   testCase_like(292, L"]%", L"aa", false);
   testCase_like(293, L"]%", L"]", true);
   testCase_like(294, L"]%", L"", false);
   testCase_like(295, L"]%", L"]h", true);
   testCase_like(296, L"]%", L"]]", true);
   testCase_like(297, L"]%", L"[", false);
   testCase_like(298, L"-%", L"-", true);
   testCase_like(299, L"-%", L"-here", true);
   testCase_like(300, L"-%", L"z", false);
   testCase_like(301, L"-%", L"", false);
   testCase_like(302, L"-%", L"dfs", false);
   testCase_like(303, L"^%", L"^", true);
   testCase_like(304, L"^%", L"^^", true);
   testCase_like(305, L"^%", L"z", false);
   testCase_like(306, L"^%", L"^another", true);
   testCase_like(307, L"^%", L"ano^ther", false);
   testCase_like(308, L"^%", L"", false);
   testCase_like(309, L"^%", L"dfs", false);
   testCase_like(310, L"-^", L"-^", true);
   testCase_like(311, L"-^", L"^-", false);
   testCase_like(312, L"-^", L"8-", false);
   testCase_like(313, L"-^", L"-8", false);
   testCase_like(314, L"-^", L"-^h", false);
   testCase_like(315, L"e#", L"e55", false);
   testCase_like(316, L"e##", L"e55", true);
   testCase_like(317, L"e%57", L"e557", true);
   testCase_like(318, L"%[c-a]%", L"co", true);
   testCase_like(319, L"%[c-a]%", L"do", false);
   testCase_like(320, L"%[c-a]%", L"c", true);
   testCase_like(321, L"%[c-a]%", L"", false);
   testCase_like(322, L"%[c-a]%", L"voine", false);
   testCase_like(323, L"%[c-a]%", L"voce", true);
   testCase_like(324, L"[b-]%", L"here", false);
   testCase_like(325, L"[b-]%", L"bear", true);
   testCase_like(326, L"[b-]%", L"cinema", false);
   testCase_like(327, L"[b-]%", L"", false);
   testCase_like(328, L"[b-]%", L"-", true);
   testCase_like(329, L"[b-]%", L"-train", true);
   testCase_like(330, L"[b-]%", L"u-train", false);
   testCase_like(331, L"[^b-]%", L"here", true);
   testCase_like(332, L"[^b-]%", L"bear", false);
   testCase_like(333, L"[^b-]%", L"cinema", true);
   testCase_like(334, L"[^b-]%", L"", false);
   testCase_like(335, L"[^b-]%", L"-", false);
   testCase_like(336, L"[^b-]%", L"-train", false);
   testCase_like(337, L"[^b-]%", L"u-train", true);
   testCase_like(338, L"#[^o]#", L"co", false);
   testCase_like(339, L"#[^o]#", L"5o9", false);
   testCase_like(340, L"#[^o]#", L"569", true);
   testCase_like(341, L"#[^o]#", L"u78", false);
   testCase_like(342, L"#[^o]#", L"b", false);
   testCase_like(343, L"#[^o]#", L"", false);
   testCase_like(344, L"#[^o]#", L"ooo", false);
   testCase_like(345, L"#[^o]#", L"667", true);
   testCase_like(346, L"#[^o]#", L"6674", false);
   testCase_like(347, L"k%_st", L"kastastast", true);
   testCase_like(348, L"k%an", L"kanan", true);
   testCase_like(349, L"k%sty", L"kastastasty", true);
   testCase_like(350, L"k%_t", L"kastastast", true);
   testCase_like(351, L"ka%_t", L"kastastast", true);

}


void testCase_like(const _int caseId, const _str& pattern, const _str& value, const _bool& expectedResult)
{
   gen::_likeptr comparer;
   gen::parseLikeCmp(comparer, pattern);
   const _bool result = comparer->compareToPattern(value);
   VERIFY(result == expectedResult, caseId);
}
