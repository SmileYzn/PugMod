<?php
include("pre.php");
$auth     = filter_input(INPUT_GET, 'auth', FILTER_SANITIZE_STRING);
$player   = (new Player)->get($auth);
$ranking  = (new PlayerStats)->getRank("player.auth = '{$auth}'");
$position = (new PlayerStats)->getPosition($auth);
$weapon   = (new WeaponStats)->getStats("player.auth = '{$auth}'");
$mapRank  = (new PlayerStats)->getMapRank($auth);
$games    = (new Game)->getGames($auth);
?>
<?php if(!empty($player['auth'])): ?>
<div class="row">
    <div class="col-md-6">
        <table class="table table-sm table-striped table-bordered text-center table-dark small">
            <tr>
                <th colspan="2"><?= $player['name'] ?></th>
            </tr>
            <tr>
                <th>Steam</th>
                <td><?= $player['auth'] ?></td>
            </tr>
            <tr>
                <th>Rank</th>
                <td><?= $position['rank'] ?> of <?= $position['ranked'] ?></td>
            </tr>
        </table>
    </div>
    <div class="col-md-6">
        <table class="table table-sm table-striped table-bordered text-center table-dark small">
            <tr>
                <th colspan="17">Server Stats</th>
            </tr>
            <tr>
                <th><abbr title="Frags">F</abbr></th>
                <th><abbr title="Assists">A</abbr></th>
                <th><abbr title="Deaths">D</abbr></th>
                <th><abbr title="Bomb Plants">BP</abbr></th>
                <th><abbr title="Bomb Defusals">BD</abbr></th>
                <th><abbr title="3 Kill Rounds">3K</abbr></th>
                <th><abbr title="4 Kill Rounds">4K</abbr></th>
                <th><abbr title="5 Kill Rounds">5K</abbr></th>
                <th><abbr title="Success in 1v2 Situations">1v2</abbr></th>
                <th><abbr title="Success in 1v3 Situations">1v3</abbr></th>
                <th><abbr title="Success in 1v4 Situations">1v4</abbr></th>
                <th><abbr title="Headshot Percentage">HSP</abbr></th>
                <th><abbr title="Average Damage Per Round">ADR</abbr></th>
                <th><abbr title="Average Frags Per Round">FPR</abbr></th>
                <th><abbr title="Average Round Win Shares">RWS</abbr></th>
            </tr>
            <?php foreach($ranking as $row): ?>
            <tr>
                <td><?= $row['kills'] ?></td>
                <td><?= $row['assist'] ?></td>
                <td><?= $row['death'] ?></td>
                <td><?= $row['bp'] ?></td>
                <td><?= $row['bd'] ?></td>
                <td><?= $row['3k'] ?></td>
                <td><?= $row['4k'] ?></td>
                <td><?= $row['5k'] ?></td>
                <td><?= $row['v2'] ?></td>
                <td><?= $row['v3'] ?></td>
                <td><?= $row['v4'] ?></td>
                <td><?= sprintf("%.3f",$row['hsp']) ?></td>
                <td><?= sprintf("%.3f",$row['adr']) ?></td>
                <td><?= sprintf("%.3f",$row['fpr']) ?></td>
                <td><?= sprintf("%.2f",$row['rws']) ?></td>
            </tr>
            <?php endforeach; ?>
        </table>
    </div>
</div>
<div class="row">
    <div class="col-md-6">
        <table class="table table-sm table-striped table-bordered text-center table-dark small">
            <tr>
                <th colspan="5">Weapon Stats</th>
            </tr>
            <tr>
                <th>Weapon</th>
                <th><abbr title="Frags">F</abbr></th>
                <th><abbr title="Deaths">D</abbr></th>
                <th><abbr title="Headshots">HS</abbr></th>
                <th><abbr title="Headshot Percentage">HSP</abbr></th>
            </tr>
            <?php foreach($weapon as $row): ?>
            <tr>
                <td><?= ucfirst($row['weapon']) ?></td>
                <td><?= $row['kills'] ?></td>
                <td><?= $row['death'] ?></td>
                <td><?= $row['hs'] ?></td>
                <td><?= sprintf("%.3f",$row['hsp']) ?></td>
            </tr>
            <?php endforeach; ?>
        </table>
    </div>
    <div class="col-md-6">
        <table class="table table-sm table-striped table-bordered text-center table-dark small">
            <tr>
                <th colspan="6">Top Maps</th>
            </tr>
            <tr>
                <th>Map</th>
                <th><abbr title="Frags">F</abbr></th>
                <th><abbr title="Assists">A</abbr></th>
                <th><abbr title="Deaths">D</abbr></th>
                <th><abbr title="Average Frags Per Round">FPR</abbr></th>
                <th><abbr title="Match Win Percentage">Win %</abbr></th>
            </tr>
            <?php foreach($mapRank as $row): ?>
            <tr>
                <td><?= $row['map'] ?></td>
                <td><?= $row['kills'] ?></td>
                <td><?= $row['assist'] ?></td>
                <td><?= $row['death'] ?></td>
                <td><?= sprintf("%.3f",$row['fpr']) ?></td>
                <td><?= sprintf("%.2f",$row['winPct']) ?></td>
            </tr>
            <?php endforeach; ?>
        </table>
    </div>
</div>
<div class="row">
    <div class="col-md-12">
        <table class="table table-sm table-striped table-bordered text-center table-dark small">
            <tr>
                <th colspan="12">Recent Matches</th>
            </tr>
            <tr>
                <th>#</th>
                <th>Map</th>
                <th>Team A</th>
                <th>Team B</th>
                <th>Result</th>
                <th>Team</th>
                <th><abbr title="Frags">F</abbr></th>
                <th><abbr title="Assists">A</abbr></th>
                <th><abbr title="Deaths">D</abbr></th>
                <th><abbr title="5 Kill Rounds">5K</abbr></th>
                <th><abbr title="Average Round Win Shares">RWS</abbr></th>
                <th>Date</th>
            </tr>
            <?php foreach($games as $row): ?>
            <tr>
                <td><?= $row['id'] ?></td>
                <td><?= $row['map'] ?></td>
                <td><?= $row['score1'] ?></td>
                <td><?= $row['score2'] ?></td>
                <td><?= $row['winner'] ? "Win" : "Loss" ?></td>
                <td><?= $row['team'] == 1 ? "Team A" : "Team B" ?></td>
                <td><?= $row['kills'] ?></td>
                <td><?= $row['assist'] ?></td>
                <td><?= $row['death'] ?></td>
                <td><?= $row['5k'] ?></td>
                <td><?= sprintf("%.2f",$row['rws']) ?></td>
                <td><?= date('d/m/Y H:i',strtotime($row['now'])) ?></td>
            </tr>
            <?php endforeach; ?>
        </table>
    </div>
</div>
<?php else: ?>
<div class="row mt-5">
    <div class="col-md-12 text-center text-white">
        <h3>Player not found.</h3>
    </div>
</div>
<?php endif; ?>
<?php
include("pro.php");
